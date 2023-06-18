
(cl:in-package :asdf)

(defsystem "agv_robot-msg"
  :depends-on (:roslisp-msg-protocol :roslisp-utils )
  :components ((:file "_package")
    (:file "car_info" :depends-on ("_package_car_info"))
    (:file "_package_car_info" :depends-on ("_package"))
  ))