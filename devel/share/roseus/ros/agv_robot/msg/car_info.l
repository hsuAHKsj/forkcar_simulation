;; Auto-generated. Do not edit!


(when (boundp 'agv_robot::car_info)
  (if (not (find-package "AGV_ROBOT"))
    (make-package "AGV_ROBOT"))
  (shadow 'car_info (find-package "AGV_ROBOT")))
(unless (find-package "AGV_ROBOT::CAR_INFO")
  (make-package "AGV_ROBOT::CAR_INFO"))

(in-package "ROS")
;;//! \htmlinclude car_info.msg.html


(defclass agv_robot::car_info
  :super ros::object
  :slots (_car_state _is_rotation_finish _index ))

(defmethod agv_robot::car_info
  (:init
   (&key
    ((:car_state __car_state) 0)
    ((:is_rotation_finish __is_rotation_finish) nil)
    ((:index __index) 0)
    )
   (send-super :init)
   (setq _car_state (round __car_state))
   (setq _is_rotation_finish __is_rotation_finish)
   (setq _index (round __index))
   self)
  (:car_state
   (&optional __car_state)
   (if __car_state (setq _car_state __car_state)) _car_state)
  (:is_rotation_finish
   (&optional __is_rotation_finish)
   (if __is_rotation_finish (setq _is_rotation_finish __is_rotation_finish)) _is_rotation_finish)
  (:index
   (&optional __index)
   (if __index (setq _index __index)) _index)
  (:serialization-length
   ()
   (+
    ;; int32 _car_state
    4
    ;; bool _is_rotation_finish
    1
    ;; int32 _index
    4
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; int32 _car_state
       (write-long _car_state s)
     ;; bool _is_rotation_finish
       (if _is_rotation_finish (write-byte -1 s) (write-byte 0 s))
     ;; int32 _index
       (write-long _index s)
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; int32 _car_state
     (setq _car_state (sys::peek buf ptr- :integer)) (incf ptr- 4)
   ;; bool _is_rotation_finish
     (setq _is_rotation_finish (not (= 0 (sys::peek buf ptr- :char)))) (incf ptr- 1)
   ;; int32 _index
     (setq _index (sys::peek buf ptr- :integer)) (incf ptr- 4)
   ;;
   self)
  )

(setf (get agv_robot::car_info :md5sum-) "b7d5607215facbb525da609deffcf62b")
(setf (get agv_robot::car_info :datatype-) "agv_robot/car_info")
(setf (get agv_robot::car_info :definition-)
      "int32 car_state #小车状态机
bool is_rotation_finish #旋转完成标志位
int32 index #第几个点

")



(provide :agv_robot/car_info "b7d5607215facbb525da609deffcf62b")


