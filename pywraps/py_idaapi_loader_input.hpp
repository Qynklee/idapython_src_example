#ifndef __PY_IDAAPI_LOADER_INPUT__
#define __PY_IDAAPI_LOADER_INPUT__

//--------------------------------------------------------------------------
//<inline(py_idaapi_loader_input)>
/*
#<pydoc>
class loader_input_t(pyidc_opaque_object_t):
    """
    A helper class to work with linput_t related functions.
    This class is also used by file loaders scripts.
    """
    def __init__(self):
        pass

    def close(self):
        """Closes the file"""
        pass

    def open(self, filename, remote = False):
        """
        Opens a file (or a remote file)
        @return: Boolean
        """
        pass

    def set_linput(self, linput):
        """Links the current loader_input_t instance to a linput_t instance"""
        pass

    @staticmethod
    def from_fp(fp):
        """A static method to construct an instance from a FILE*"""
        pass

    def open_memory(self, start, size):
        """
        Create a linput for process memory (By internally calling idaapi.create_memory_linput())
        This linput will use dbg->read_memory() to read data
        @param start: starting address of the input
        @param size: size of the memory range to represent as linput
                    if unknown, may be passed as 0
        """
        pass

    def seek(self, pos, whence = SEEK_SET):
        """
        Set input source position
        @return: the new position (not 0 as fseek!)
        """
        pass

    def tell(self):
        """Returns the current position"""
        pass

    def getz(self, sz, fpos = -1):
        """
        Returns a zero terminated string at the given position
        @param sz: maximum size of the string
        @param fpos: if != -1 then seek will be performed before reading
        @return: The string or None on failure.
        """
        pass

    def gets(self, len):
        """Reads a line from the input file. Returns the read line or None"""
        pass

    def read(self, size):
        """Reads from the file. Returns the buffer or None"""
        pass

    def readbytes(self, size, big_endian):
        """Similar to read() but it respect the endianness"""
        pass

    def file2base(self, pos, ea1, ea2, patchable):
        """
        Load portion of file into the database
        This function will include (ea1..ea2) into the addressing space of the
        program (make it enabled)
        @param li: pointer ot input source
        @param pos: position in the file
        @param (ea1..ea2): range of destination linear addresses
        @param patchable: should the kernel remember correspondance of
                          file offsets to linear addresses.
        @return: 1-ok,0-read error, a warning is displayed
        """
        pass

    def get_byte(self):
        """Reads a single byte from the file. Returns None if EOF or the read byte"""
        pass

    def opened(self):
        """Checks if the file is opened or not"""
        pass
#</pydoc>
*/
class loader_input_t
{
private:
  linput_t *li;
  int own;
  qstring fn;
  enum
  {
    OWN_NONE    = 0, // li not created yet
    OWN_CREATE  = 1, // Owns li because we created it
    OWN_FROM_LI = 2, // No ownership we borrowed the li from another class
    OWN_FROM_FP = 3, // We got an li instance from an fp instance, we have to unmake_linput() on Close
  };

  //--------------------------------------------------------------------------
  void _from_capsule(PyObject *pycapsule)
  {
    PYW_GIL_CHECK_LOCKED_SCOPE();
    this->set_linput((linput_t *)PyCapsule_GetPointer(pycapsule, VALID_CAPSULE_NAME));
  }

  //--------------------------------------------------------------------------
  void assign(const loader_input_t &rhs)
  {
    fn = rhs.fn;
    li = rhs.li;
    own = OWN_FROM_LI;
  }

  //--------------------------------------------------------------------------
  loader_input_t(const loader_input_t &rhs)
  {
    assign(rhs);
  }
public:
  // Special attribute that tells the pyvar_to_idcvar how to convert this
  // class from and to IDC. The value of this variable must be set to two
  int __idc_cvt_id__;
  //--------------------------------------------------------------------------
  loader_input_t(PyObject *pycapsule = nullptr): li(nullptr), own(OWN_NONE), __idc_cvt_id__(PY_ICID_OPAQUE)
  {
    PYW_GIL_CHECK_LOCKED_SCOPE();
    if ( pycapsule != nullptr && PyCapsule_IsValid(pycapsule, VALID_CAPSULE_NAME) )
      _from_capsule(pycapsule);
  }

  //--------------------------------------------------------------------------
  void close()
  {
    if ( li == nullptr )
      return;

    PYW_GIL_GET;
    Py_BEGIN_ALLOW_THREADS;
    if ( own == OWN_CREATE )
      close_linput(li);
    else if ( own == OWN_FROM_FP )
      unmake_linput(li);
    Py_END_ALLOW_THREADS;
    li = nullptr;
    own = OWN_NONE;
  }

  //--------------------------------------------------------------------------
  ~loader_input_t()
  {
    close();
  }

  //--------------------------------------------------------------------------
  bool open(const char *filename, bool remote = false)
  {
    close();
    PYW_GIL_GET;
    Py_BEGIN_ALLOW_THREADS;
    li = open_linput(filename, remote);
    if ( li != nullptr )
    {
      // Save file name
      fn = filename;
      own = OWN_CREATE;
    }
    Py_END_ALLOW_THREADS;
    return li != nullptr;
  }

  //--------------------------------------------------------------------------
  void set_linput(linput_t *linput)
  {
    close();
    own = OWN_FROM_LI;
    li = linput;
    fn.sprnt("<linput_t * %p>", linput);
  }

  //--------------------------------------------------------------------------
  static loader_input_t *from_linput(linput_t *linput)
  {
    loader_input_t *l = new loader_input_t();
    l->set_linput(linput);
    return l;
  }

  //--------------------------------------------------------------------------
  // This method can be used to pass a linput_t* from C code
  static loader_input_t *from_capsule(PyObject *pycapsule)
  {
    PYW_GIL_CHECK_LOCKED_SCOPE();
    if ( !PyCapsule_IsValid(pycapsule, VALID_CAPSULE_NAME) )
      return nullptr;
    loader_input_t *l = new loader_input_t();
    l->_from_capsule(pycapsule);
    return l;
  }

  //--------------------------------------------------------------------------
  static loader_input_t *from_fp(FILE *fp)
  {
    PYW_GIL_GET;
    loader_input_t *l = nullptr;
    Py_BEGIN_ALLOW_THREADS;
    linput_t *fp_li = make_linput(fp);
    if ( fp_li != nullptr )
    {
      l = new loader_input_t();
      l->own = OWN_FROM_FP;
      l->fn.sprnt("<FILE * %p>", fp);
      l->li = fp_li;
    }
    Py_END_ALLOW_THREADS;
    return l;
  }

  //--------------------------------------------------------------------------
  linput_t *get_linput()
  {
    return li;
  }

  //--------------------------------------------------------------------------
  bool open_memory(ea_t start, asize_t size = 0)
  {
    PYW_GIL_GET;
    linput_t *l;
    Py_BEGIN_ALLOW_THREADS;
    l = create_memory_linput(start, size);
    if ( l != nullptr )
    {
      close();
      li = l;
      fn = "<memory>";
      own = OWN_CREATE;
    }
    Py_END_ALLOW_THREADS;
    return l != nullptr;
  }

  //--------------------------------------------------------------------------
  int64 seek(int64 pos, int whence = SEEK_SET)
  {
    int64 r;
    PYW_GIL_GET;
    Py_BEGIN_ALLOW_THREADS;
    r = qlseek(li, pos, whence);
    Py_END_ALLOW_THREADS;
    return r;
  }

  //--------------------------------------------------------------------------
  int64 tell()
  {
    int64 r;
    PYW_GIL_GET;
    Py_BEGIN_ALLOW_THREADS;
    r = qltell(li);
    Py_END_ALLOW_THREADS;
    return r;
  }

  //--------------------------------------------------------------------------
  PyObject *getz(size_t sz, int64 fpos = -1)
  {
    PYW_GIL_CHECK_LOCKED_SCOPE();
    do
    {
      char *buf = (char *) malloc(sz + 5);
      if ( buf == nullptr )
        break;
      Py_BEGIN_ALLOW_THREADS;
      qlgetz(li, fpos, buf, sz);
      Py_END_ALLOW_THREADS;
      PyObject *ret = IDAPyStr_FromUTF8(buf);
      free(buf);
      return ret;
    } while ( false );
    Py_RETURN_NONE;
  }

  //--------------------------------------------------------------------------
  PyObject *gets(size_t len)
  {
    PYW_GIL_CHECK_LOCKED_SCOPE();
    bytevec_t buf;
    buf.resize(len);
    bool ok;
    Py_BEGIN_ALLOW_THREADS;
    ok = qlgets((char *) buf.begin(), buf.size(), li) != nullptr;
    Py_END_ALLOW_THREADS;
    if ( !ok )
      Py_RETURN_NONE;
    return IDAPyStr_FromUTF8((const char *) buf.begin());
  }

  //--------------------------------------------------------------------------
  PyObject *read(size_t size)
  {
    PYW_GIL_CHECK_LOCKED_SCOPE();
    bytevec_t buf;
    buf.resize(size);
    ssize_t nread;
    Py_BEGIN_ALLOW_THREADS;
    nread = qlread(li, (char *) buf.begin(), buf.size());
    Py_END_ALLOW_THREADS;
    if ( nread <= 0 )
      Py_RETURN_NONE;
    return IDAPyBytes_FromMemAndSize((const char *) buf.begin(), nread);
  }

  //--------------------------------------------------------------------------
  bool opened()
  {
    return li != nullptr;
  }

  //--------------------------------------------------------------------------
  PyObject *readbytes(size_t size, bool big_endian)
  {
    PYW_GIL_CHECK_LOCKED_SCOPE();
    do
    {
      char *buf = (char *) malloc(size + 5);
      if ( buf == nullptr )
        break;
      int r;
      Py_BEGIN_ALLOW_THREADS;
      r = lreadbytes(li, buf, size, big_endian);
      Py_END_ALLOW_THREADS;
      if ( r == -1 )
        r = 0;
      PyObject *ret = IDAPyStr_FromUTF8AndSize(buf, r);
      free(buf);
      return ret;
    } while ( false );
    Py_RETURN_NONE;
  }

  //--------------------------------------------------------------------------
  int file2base(int64 pos, ea_t ea1, ea_t ea2, int patchable)
  {
    int rc;
    Py_BEGIN_ALLOW_THREADS;
    rc = ::file2base(li, pos, ea1, ea2, patchable);
    Py_END_ALLOW_THREADS;
    return rc;
  }

  //--------------------------------------------------------------------------
  int64 size()
  {
    int64 rc;
    Py_BEGIN_ALLOW_THREADS;
    rc = qlsize(li);
    Py_END_ALLOW_THREADS;
    return rc;
  }

  //--------------------------------------------------------------------------
  PyObject *filename()
  {
    PYW_GIL_CHECK_LOCKED_SCOPE();
    return IDAPyStr_FromUTF8(fn.c_str());
  }

  //--------------------------------------------------------------------------
#ifdef PY3
  PyObject *get_byte()
#else
  PyObject *get_char()
#endif
  {
    PYW_GIL_CHECK_LOCKED_SCOPE();
    int ch;
    Py_BEGIN_ALLOW_THREADS;
    ch = qlgetc(li);
    Py_END_ALLOW_THREADS;
    if ( ch == EOF )
      Py_RETURN_NONE;
#ifdef PY3
    return Py_BuildValue("i", ch);
#else
    return Py_BuildValue("c", ch);
#endif
  }
};
//</inline(py_idaapi_loader_input)>

#endif // __PY_IDAAPI_LOADER_INPUT__
