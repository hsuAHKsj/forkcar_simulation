# This Python file uses the following encoding: utf-8
"""autogenerated by genpy from agv_robot/car_info.msg. Do not edit."""
import codecs
import sys
python3 = True if sys.hexversion > 0x03000000 else False
import genpy
import struct


class car_info(genpy.Message):
  _md5sum = "b7d5607215facbb525da609deffcf62b"
  _type = "agv_robot/car_info"
  _has_header = False  # flag to mark the presence of a Header object
  _full_text = """int32 car_state #小车状态机
bool is_rotation_finish #旋转完成标志位
int32 index #第几个点
"""
  __slots__ = ['car_state','is_rotation_finish','index']
  _slot_types = ['int32','bool','int32']

  def __init__(self, *args, **kwds):
    """
    Constructor. Any message fields that are implicitly/explicitly
    set to None will be assigned a default value. The recommend
    use is keyword arguments as this is more robust to future message
    changes.  You cannot mix in-order arguments and keyword arguments.

    The available fields are:
       car_state,is_rotation_finish,index

    :param args: complete set of field values, in .msg order
    :param kwds: use keyword arguments corresponding to message field names
    to set specific fields.
    """
    if args or kwds:
      super(car_info, self).__init__(*args, **kwds)
      # message fields cannot be None, assign default values for those that are
      if self.car_state is None:
        self.car_state = 0
      if self.is_rotation_finish is None:
        self.is_rotation_finish = False
      if self.index is None:
        self.index = 0
    else:
      self.car_state = 0
      self.is_rotation_finish = False
      self.index = 0

  def _get_types(self):
    """
    internal API method
    """
    return self._slot_types

  def serialize(self, buff):
    """
    serialize message into buffer
    :param buff: buffer, ``StringIO``
    """
    try:
      _x = self
      buff.write(_get_struct_iBi().pack(_x.car_state, _x.is_rotation_finish, _x.index))
    except struct.error as se: self._check_types(struct.error("%s: '%s' when writing '%s'" % (type(se), str(se), str(locals().get('_x', self)))))
    except TypeError as te: self._check_types(ValueError("%s: '%s' when writing '%s'" % (type(te), str(te), str(locals().get('_x', self)))))

  def deserialize(self, str):
    """
    unpack serialized message in str into this message instance
    :param str: byte array of serialized message, ``str``
    """
    if python3:
      codecs.lookup_error("rosmsg").msg_type = self._type
    try:
      end = 0
      _x = self
      start = end
      end += 9
      (_x.car_state, _x.is_rotation_finish, _x.index,) = _get_struct_iBi().unpack(str[start:end])
      self.is_rotation_finish = bool(self.is_rotation_finish)
      return self
    except struct.error as e:
      raise genpy.DeserializationError(e)  # most likely buffer underfill


  def serialize_numpy(self, buff, numpy):
    """
    serialize message with numpy array types into buffer
    :param buff: buffer, ``StringIO``
    :param numpy: numpy python module
    """
    try:
      _x = self
      buff.write(_get_struct_iBi().pack(_x.car_state, _x.is_rotation_finish, _x.index))
    except struct.error as se: self._check_types(struct.error("%s: '%s' when writing '%s'" % (type(se), str(se), str(locals().get('_x', self)))))
    except TypeError as te: self._check_types(ValueError("%s: '%s' when writing '%s'" % (type(te), str(te), str(locals().get('_x', self)))))

  def deserialize_numpy(self, str, numpy):
    """
    unpack serialized message in str into this message instance using numpy for array types
    :param str: byte array of serialized message, ``str``
    :param numpy: numpy python module
    """
    if python3:
      codecs.lookup_error("rosmsg").msg_type = self._type
    try:
      end = 0
      _x = self
      start = end
      end += 9
      (_x.car_state, _x.is_rotation_finish, _x.index,) = _get_struct_iBi().unpack(str[start:end])
      self.is_rotation_finish = bool(self.is_rotation_finish)
      return self
    except struct.error as e:
      raise genpy.DeserializationError(e)  # most likely buffer underfill

_struct_I = genpy.struct_I
def _get_struct_I():
    global _struct_I
    return _struct_I
_struct_iBi = None
def _get_struct_iBi():
    global _struct_iBi
    if _struct_iBi is None:
        _struct_iBi = struct.Struct("<iBi")
    return _struct_iBi
