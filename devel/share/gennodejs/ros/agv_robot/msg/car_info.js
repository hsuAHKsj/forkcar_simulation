// Auto-generated. Do not edit!

// (in-package agv_robot.msg)


"use strict";

const _serializer = _ros_msg_utils.Serialize;
const _arraySerializer = _serializer.Array;
const _deserializer = _ros_msg_utils.Deserialize;
const _arrayDeserializer = _deserializer.Array;
const _finder = _ros_msg_utils.Find;
const _getByteLength = _ros_msg_utils.getByteLength;

//-----------------------------------------------------------

class car_info {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.car_state = null;
      this.is_rotation_finish = null;
      this.index = null;
    }
    else {
      if (initObj.hasOwnProperty('car_state')) {
        this.car_state = initObj.car_state
      }
      else {
        this.car_state = 0;
      }
      if (initObj.hasOwnProperty('is_rotation_finish')) {
        this.is_rotation_finish = initObj.is_rotation_finish
      }
      else {
        this.is_rotation_finish = false;
      }
      if (initObj.hasOwnProperty('index')) {
        this.index = initObj.index
      }
      else {
        this.index = 0;
      }
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type car_info
    // Serialize message field [car_state]
    bufferOffset = _serializer.int32(obj.car_state, buffer, bufferOffset);
    // Serialize message field [is_rotation_finish]
    bufferOffset = _serializer.bool(obj.is_rotation_finish, buffer, bufferOffset);
    // Serialize message field [index]
    bufferOffset = _serializer.int32(obj.index, buffer, bufferOffset);
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type car_info
    let len;
    let data = new car_info(null);
    // Deserialize message field [car_state]
    data.car_state = _deserializer.int32(buffer, bufferOffset);
    // Deserialize message field [is_rotation_finish]
    data.is_rotation_finish = _deserializer.bool(buffer, bufferOffset);
    // Deserialize message field [index]
    data.index = _deserializer.int32(buffer, bufferOffset);
    return data;
  }

  static getMessageSize(object) {
    return 9;
  }

  static datatype() {
    // Returns string type for a message object
    return 'agv_robot/car_info';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return 'b7d5607215facbb525da609deffcf62b';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    int32 car_state #小车状态机
    bool is_rotation_finish #旋转完成标志位
    int32 index #第几个点
    
    `;
  }

  static Resolve(msg) {
    // deep-construct a valid message object instance of whatever was passed in
    if (typeof msg !== 'object' || msg === null) {
      msg = {};
    }
    const resolved = new car_info(null);
    if (msg.car_state !== undefined) {
      resolved.car_state = msg.car_state;
    }
    else {
      resolved.car_state = 0
    }

    if (msg.is_rotation_finish !== undefined) {
      resolved.is_rotation_finish = msg.is_rotation_finish;
    }
    else {
      resolved.is_rotation_finish = false
    }

    if (msg.index !== undefined) {
      resolved.index = msg.index;
    }
    else {
      resolved.index = 0
    }

    return resolved;
    }
};

module.exports = car_info;
