; Auto-generated. Do not edit!


(cl:in-package agv_robot-msg)


;//! \htmlinclude car_info.msg.html

(cl:defclass <car_info> (roslisp-msg-protocol:ros-message)
  ((car_state
    :reader car_state
    :initarg :car_state
    :type cl:integer
    :initform 0)
   (is_rotation_finish
    :reader is_rotation_finish
    :initarg :is_rotation_finish
    :type cl:boolean
    :initform cl:nil)
   (index
    :reader index
    :initarg :index
    :type cl:integer
    :initform 0))
)

(cl:defclass car_info (<car_info>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <car_info>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'car_info)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name agv_robot-msg:<car_info> is deprecated: use agv_robot-msg:car_info instead.")))

(cl:ensure-generic-function 'car_state-val :lambda-list '(m))
(cl:defmethod car_state-val ((m <car_info>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader agv_robot-msg:car_state-val is deprecated.  Use agv_robot-msg:car_state instead.")
  (car_state m))

(cl:ensure-generic-function 'is_rotation_finish-val :lambda-list '(m))
(cl:defmethod is_rotation_finish-val ((m <car_info>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader agv_robot-msg:is_rotation_finish-val is deprecated.  Use agv_robot-msg:is_rotation_finish instead.")
  (is_rotation_finish m))

(cl:ensure-generic-function 'index-val :lambda-list '(m))
(cl:defmethod index-val ((m <car_info>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader agv_robot-msg:index-val is deprecated.  Use agv_robot-msg:index instead.")
  (index m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <car_info>) ostream)
  "Serializes a message object of type '<car_info>"
  (cl:let* ((signed (cl:slot-value msg 'car_state)) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 4294967296) signed)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) unsigned) ostream)
    )
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:if (cl:slot-value msg 'is_rotation_finish) 1 0)) ostream)
  (cl:let* ((signed (cl:slot-value msg 'index)) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 4294967296) signed)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) unsigned) ostream)
    )
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <car_info>) istream)
  "Deserializes a message object of type '<car_info>"
    (cl:let ((unsigned 0))
      (cl:setf (cl:ldb (cl:byte 8 0) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) unsigned) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'car_state) (cl:if (cl:< unsigned 2147483648) unsigned (cl:- unsigned 4294967296))))
    (cl:setf (cl:slot-value msg 'is_rotation_finish) (cl:not (cl:zerop (cl:read-byte istream))))
    (cl:let ((unsigned 0))
      (cl:setf (cl:ldb (cl:byte 8 0) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) unsigned) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'index) (cl:if (cl:< unsigned 2147483648) unsigned (cl:- unsigned 4294967296))))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<car_info>)))
  "Returns string type for a message object of type '<car_info>"
  "agv_robot/car_info")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'car_info)))
  "Returns string type for a message object of type 'car_info"
  "agv_robot/car_info")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<car_info>)))
  "Returns md5sum for a message object of type '<car_info>"
  "b7d5607215facbb525da609deffcf62b")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'car_info)))
  "Returns md5sum for a message object of type 'car_info"
  "b7d5607215facbb525da609deffcf62b")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<car_info>)))
  "Returns full string definition for message of type '<car_info>"
  (cl:format cl:nil "int32 car_state #小车状态机~%bool is_rotation_finish #旋转完成标志位~%int32 index #第几个点~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'car_info)))
  "Returns full string definition for message of type 'car_info"
  (cl:format cl:nil "int32 car_state #小车状态机~%bool is_rotation_finish #旋转完成标志位~%int32 index #第几个点~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <car_info>))
  (cl:+ 0
     4
     1
     4
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <car_info>))
  "Converts a ROS message object to a list"
  (cl:list 'car_info
    (cl:cons ':car_state (car_state msg))
    (cl:cons ':is_rotation_finish (is_rotation_finish msg))
    (cl:cons ':index (index msg))
))
