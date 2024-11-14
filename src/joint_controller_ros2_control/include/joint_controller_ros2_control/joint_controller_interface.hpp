#ifndef JOINT_CONTROLLER_INTERFACE_HPP_
#define JOINT_CONTROLLER_INTERFACE_HPP_

#include <memory>
#include <string>
#include <vector>

#include "joint_controller_msgs/msg/joint_command.hpp"
#include "sensor_msgs/msg/joint_state.hpp"
#include "controller_interface/chainable_controller_interface.hpp"
#include "controller_interface/controller_interface.hpp"
#include "rclcpp_lifecycle/state.hpp"
#include "realtime_tools/realtime_buffer.h"
#include "realtime_tools/realtime_publisher.h"

#include "joint_controller_ros2_control/joint_controller_core.hpp"
#include "joint_controller_ros2_control/visibility_control.hpp"
#include "joint_controller_parameters.hpp"

namespace joint_controller_interface
{

  class JointControllerInterface : public controller_interface::ChainableControllerInterface
  {

    public:

    JOINT_CONTROLLER_INTERFACE__VISIBILITY_PUBLIC 
    JointControllerInterface();

    JOINT_CONTROLLER_INTERFACE__VISIBILITY_PUBLIC 
    controller_interface::CallbackReturn on_init() override;

    JOINT_CONTROLLER_INTERFACE__VISIBILITY_PUBLIC 
    controller_interface::InterfaceConfiguration command_interface_configuration() const override;

    JOINT_CONTROLLER_INTERFACE__VISIBILITY_PUBLIC 
    controller_interface::InterfaceConfiguration state_interface_configuration() const override;

    JOINT_CONTROLLER_INTERFACE__VISIBILITY_PUBLIC 
    controller_interface::CallbackReturn on_cleanup(
      const rclcpp_lifecycle::State & previous_state) override;

    JOINT_CONTROLLER_INTERFACE__VISIBILITY_PUBLIC 
    controller_interface::CallbackReturn on_configure(
      const rclcpp_lifecycle::State & previous_state) override;

    JOINT_CONTROLLER_INTERFACE__VISIBILITY_PUBLIC 
    controller_interface::CallbackReturn on_activate(
      const rclcpp_lifecycle::State & previous_state) override;

    JOINT_CONTROLLER_INTERFACE__VISIBILITY_PUBLIC 
    controller_interface::CallbackReturn on_deactivate(
      const rclcpp_lifecycle::State & previous_state) override;

    JOINT_CONTROLLER_INTERFACE__VISIBILITY_PUBLIC 
    controller_interface::return_type update_reference_from_subscribers(
      const rclcpp::Time & time, const rclcpp::Duration & period) override;

    JOINT_CONTROLLER_INTERFACE__VISIBILITY_PUBLIC 
    controller_interface::return_type update_and_write_commands(
      const rclcpp::Time & time, const rclcpp::Duration & period) override;

    protected:
    std::shared_ptr<joint_controller_parameters::ParamListener> param_listener_; // TODO DODAJ TO
    joint_controller_parameters::Params params_;

    int joint_num_;
    std::vector<std::string> joint_names_;

    using JointCommands = joint_controller_core::JointCommands;
    using JointStates = joint_controller_core::JointStates;

    std::unordered_map<std::string, JointCommands> joint_commands_;
    std::unordered_map<std::string, JointStates> joint_states_;


    using JointController = joint_controller_core::JointController;
    std::vector<JointController> joint_controllers_;
    
    using JointCommandMsg = joint_controller_msgs::msg::JointCommand;
    using JointStateMsg = sensor_msgs::msg::JointState;

    // Command subscribers and Controller State publisher
    rclcpp::Subscription<JointCommandMsg>::SharedPtr command_subscriber_ = nullptr;
    realtime_tools::RealtimeBuffer<std::shared_ptr<JointCommandMsg>> input_commands_;

    bool has_kp_scale_interface_ = false;
    bool has_kd_scale_interface_ = false;

    std::vector<std::string> default_state_interfaces_{"position", "velocity"};
    std::vector<std::string> command_interface_joint_command_map_{"effort"};

    std::unordered_map<std::string, std::unordered_map<std::string, size_t>> state_interface_joint_state_map_;
    std::unordered_map<std::string, std::unordered_map<std::string, size_t>> command_interface_joint_command_map_;
    
    // override methods from ChainableControllerInterface
    std::vector<hardware_interface::CommandInterface> on_export_reference_interfaces() override;

    std::vector<hardware_interface::StateInterface> on_export_state_interfaces() override;

    bool on_set_chained_mode(bool chained_mode) override;

    // internal methods
    controller_interface::CallbackReturn configure_joints();

    void command_callback(const std::shared_ptr<JointCommandMsg> _command_msg);

    controller_interface::CallbackReturn get_state_interfaces_map();
    controller_interface::CallbackReturn get_command_interfaces_map();

    void JointControllerInterface::reset_controller_command_msg(
      const std::shared_ptr<JointCommandMsg>& _msg, const std::vector<std::string> & _joint_names);

    private:
  };
};  // namespace pid_controller

#endif  // PID_CONTROLLER__PID_CONTROLLER_HPP_