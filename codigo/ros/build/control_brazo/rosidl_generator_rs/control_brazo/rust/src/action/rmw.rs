
#[cfg(feature = "serde")]
use serde::{Deserialize, Serialize};


#[link(name = "control_brazo__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__control_brazo__action__MoverBrazo_Goal() -> *const std::ffi::c_void;
}

#[link(name = "control_brazo__rosidl_generator_c")]
extern "C" {
    fn control_brazo__action__MoverBrazo_Goal__init(msg: *mut MoverBrazo_Goal) -> bool;
    fn control_brazo__action__MoverBrazo_Goal__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<MoverBrazo_Goal>, size: usize) -> bool;
    fn control_brazo__action__MoverBrazo_Goal__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<MoverBrazo_Goal>);
    fn control_brazo__action__MoverBrazo_Goal__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<MoverBrazo_Goal>, out_seq: *mut rosidl_runtime_rs::Sequence<MoverBrazo_Goal>) -> bool;
}

// Corresponds to control_brazo__action__MoverBrazo_Goal
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct MoverBrazo_Goal {

    // This member is not documented.
    #[allow(missing_docs)]
    pub x: f32,


    // This member is not documented.
    #[allow(missing_docs)]
    pub y: f32,


    // This member is not documented.
    #[allow(missing_docs)]
    pub z: f32,

}



impl Default for MoverBrazo_Goal {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !control_brazo__action__MoverBrazo_Goal__init(&mut msg as *mut _) {
        panic!("Call to control_brazo__action__MoverBrazo_Goal__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for MoverBrazo_Goal {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { control_brazo__action__MoverBrazo_Goal__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { control_brazo__action__MoverBrazo_Goal__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { control_brazo__action__MoverBrazo_Goal__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for MoverBrazo_Goal {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for MoverBrazo_Goal where Self: Sized {
  const TYPE_NAME: &'static str = "control_brazo/action/MoverBrazo_Goal";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__control_brazo__action__MoverBrazo_Goal() }
  }
}


#[link(name = "control_brazo__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__control_brazo__action__MoverBrazo_Result() -> *const std::ffi::c_void;
}

#[link(name = "control_brazo__rosidl_generator_c")]
extern "C" {
    fn control_brazo__action__MoverBrazo_Result__init(msg: *mut MoverBrazo_Result) -> bool;
    fn control_brazo__action__MoverBrazo_Result__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<MoverBrazo_Result>, size: usize) -> bool;
    fn control_brazo__action__MoverBrazo_Result__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<MoverBrazo_Result>);
    fn control_brazo__action__MoverBrazo_Result__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<MoverBrazo_Result>, out_seq: *mut rosidl_runtime_rs::Sequence<MoverBrazo_Result>) -> bool;
}

// Corresponds to control_brazo__action__MoverBrazo_Result
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct MoverBrazo_Result {

    // This member is not documented.
    #[allow(missing_docs)]
    pub exito: bool,


    // This member is not documented.
    #[allow(missing_docs)]
    pub mensaje: rosidl_runtime_rs::String,

}



impl Default for MoverBrazo_Result {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !control_brazo__action__MoverBrazo_Result__init(&mut msg as *mut _) {
        panic!("Call to control_brazo__action__MoverBrazo_Result__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for MoverBrazo_Result {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { control_brazo__action__MoverBrazo_Result__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { control_brazo__action__MoverBrazo_Result__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { control_brazo__action__MoverBrazo_Result__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for MoverBrazo_Result {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for MoverBrazo_Result where Self: Sized {
  const TYPE_NAME: &'static str = "control_brazo/action/MoverBrazo_Result";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__control_brazo__action__MoverBrazo_Result() }
  }
}


#[link(name = "control_brazo__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__control_brazo__action__MoverBrazo_Feedback() -> *const std::ffi::c_void;
}

#[link(name = "control_brazo__rosidl_generator_c")]
extern "C" {
    fn control_brazo__action__MoverBrazo_Feedback__init(msg: *mut MoverBrazo_Feedback) -> bool;
    fn control_brazo__action__MoverBrazo_Feedback__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<MoverBrazo_Feedback>, size: usize) -> bool;
    fn control_brazo__action__MoverBrazo_Feedback__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<MoverBrazo_Feedback>);
    fn control_brazo__action__MoverBrazo_Feedback__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<MoverBrazo_Feedback>, out_seq: *mut rosidl_runtime_rs::Sequence<MoverBrazo_Feedback>) -> bool;
}

// Corresponds to control_brazo__action__MoverBrazo_Feedback
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct MoverBrazo_Feedback {

    // This member is not documented.
    #[allow(missing_docs)]
    pub progreso: f32,

}



impl Default for MoverBrazo_Feedback {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !control_brazo__action__MoverBrazo_Feedback__init(&mut msg as *mut _) {
        panic!("Call to control_brazo__action__MoverBrazo_Feedback__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for MoverBrazo_Feedback {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { control_brazo__action__MoverBrazo_Feedback__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { control_brazo__action__MoverBrazo_Feedback__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { control_brazo__action__MoverBrazo_Feedback__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for MoverBrazo_Feedback {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for MoverBrazo_Feedback where Self: Sized {
  const TYPE_NAME: &'static str = "control_brazo/action/MoverBrazo_Feedback";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__control_brazo__action__MoverBrazo_Feedback() }
  }
}


#[link(name = "control_brazo__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__control_brazo__action__MoverBrazo_FeedbackMessage() -> *const std::ffi::c_void;
}

#[link(name = "control_brazo__rosidl_generator_c")]
extern "C" {
    fn control_brazo__action__MoverBrazo_FeedbackMessage__init(msg: *mut MoverBrazo_FeedbackMessage) -> bool;
    fn control_brazo__action__MoverBrazo_FeedbackMessage__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<MoverBrazo_FeedbackMessage>, size: usize) -> bool;
    fn control_brazo__action__MoverBrazo_FeedbackMessage__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<MoverBrazo_FeedbackMessage>);
    fn control_brazo__action__MoverBrazo_FeedbackMessage__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<MoverBrazo_FeedbackMessage>, out_seq: *mut rosidl_runtime_rs::Sequence<MoverBrazo_FeedbackMessage>) -> bool;
}

// Corresponds to control_brazo__action__MoverBrazo_FeedbackMessage
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct MoverBrazo_FeedbackMessage {

    // This member is not documented.
    #[allow(missing_docs)]
    pub goal_id: unique_identifier_msgs::msg::rmw::UUID,


    // This member is not documented.
    #[allow(missing_docs)]
    pub feedback: super::super::action::rmw::MoverBrazo_Feedback,

}



impl Default for MoverBrazo_FeedbackMessage {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !control_brazo__action__MoverBrazo_FeedbackMessage__init(&mut msg as *mut _) {
        panic!("Call to control_brazo__action__MoverBrazo_FeedbackMessage__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for MoverBrazo_FeedbackMessage {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { control_brazo__action__MoverBrazo_FeedbackMessage__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { control_brazo__action__MoverBrazo_FeedbackMessage__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { control_brazo__action__MoverBrazo_FeedbackMessage__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for MoverBrazo_FeedbackMessage {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for MoverBrazo_FeedbackMessage where Self: Sized {
  const TYPE_NAME: &'static str = "control_brazo/action/MoverBrazo_FeedbackMessage";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__control_brazo__action__MoverBrazo_FeedbackMessage() }
  }
}




#[link(name = "control_brazo__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__control_brazo__action__MoverBrazo_SendGoal_Request() -> *const std::ffi::c_void;
}

#[link(name = "control_brazo__rosidl_generator_c")]
extern "C" {
    fn control_brazo__action__MoverBrazo_SendGoal_Request__init(msg: *mut MoverBrazo_SendGoal_Request) -> bool;
    fn control_brazo__action__MoverBrazo_SendGoal_Request__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<MoverBrazo_SendGoal_Request>, size: usize) -> bool;
    fn control_brazo__action__MoverBrazo_SendGoal_Request__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<MoverBrazo_SendGoal_Request>);
    fn control_brazo__action__MoverBrazo_SendGoal_Request__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<MoverBrazo_SendGoal_Request>, out_seq: *mut rosidl_runtime_rs::Sequence<MoverBrazo_SendGoal_Request>) -> bool;
}

// Corresponds to control_brazo__action__MoverBrazo_SendGoal_Request
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct MoverBrazo_SendGoal_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub goal_id: unique_identifier_msgs::msg::rmw::UUID,


    // This member is not documented.
    #[allow(missing_docs)]
    pub goal: super::super::action::rmw::MoverBrazo_Goal,

}



impl Default for MoverBrazo_SendGoal_Request {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !control_brazo__action__MoverBrazo_SendGoal_Request__init(&mut msg as *mut _) {
        panic!("Call to control_brazo__action__MoverBrazo_SendGoal_Request__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for MoverBrazo_SendGoal_Request {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { control_brazo__action__MoverBrazo_SendGoal_Request__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { control_brazo__action__MoverBrazo_SendGoal_Request__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { control_brazo__action__MoverBrazo_SendGoal_Request__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for MoverBrazo_SendGoal_Request {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for MoverBrazo_SendGoal_Request where Self: Sized {
  const TYPE_NAME: &'static str = "control_brazo/action/MoverBrazo_SendGoal_Request";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__control_brazo__action__MoverBrazo_SendGoal_Request() }
  }
}


#[link(name = "control_brazo__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__control_brazo__action__MoverBrazo_SendGoal_Response() -> *const std::ffi::c_void;
}

#[link(name = "control_brazo__rosidl_generator_c")]
extern "C" {
    fn control_brazo__action__MoverBrazo_SendGoal_Response__init(msg: *mut MoverBrazo_SendGoal_Response) -> bool;
    fn control_brazo__action__MoverBrazo_SendGoal_Response__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<MoverBrazo_SendGoal_Response>, size: usize) -> bool;
    fn control_brazo__action__MoverBrazo_SendGoal_Response__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<MoverBrazo_SendGoal_Response>);
    fn control_brazo__action__MoverBrazo_SendGoal_Response__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<MoverBrazo_SendGoal_Response>, out_seq: *mut rosidl_runtime_rs::Sequence<MoverBrazo_SendGoal_Response>) -> bool;
}

// Corresponds to control_brazo__action__MoverBrazo_SendGoal_Response
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct MoverBrazo_SendGoal_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub accepted: bool,


    // This member is not documented.
    #[allow(missing_docs)]
    pub stamp: builtin_interfaces::msg::rmw::Time,

}



impl Default for MoverBrazo_SendGoal_Response {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !control_brazo__action__MoverBrazo_SendGoal_Response__init(&mut msg as *mut _) {
        panic!("Call to control_brazo__action__MoverBrazo_SendGoal_Response__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for MoverBrazo_SendGoal_Response {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { control_brazo__action__MoverBrazo_SendGoal_Response__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { control_brazo__action__MoverBrazo_SendGoal_Response__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { control_brazo__action__MoverBrazo_SendGoal_Response__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for MoverBrazo_SendGoal_Response {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for MoverBrazo_SendGoal_Response where Self: Sized {
  const TYPE_NAME: &'static str = "control_brazo/action/MoverBrazo_SendGoal_Response";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__control_brazo__action__MoverBrazo_SendGoal_Response() }
  }
}


#[link(name = "control_brazo__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__control_brazo__action__MoverBrazo_GetResult_Request() -> *const std::ffi::c_void;
}

#[link(name = "control_brazo__rosidl_generator_c")]
extern "C" {
    fn control_brazo__action__MoverBrazo_GetResult_Request__init(msg: *mut MoverBrazo_GetResult_Request) -> bool;
    fn control_brazo__action__MoverBrazo_GetResult_Request__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<MoverBrazo_GetResult_Request>, size: usize) -> bool;
    fn control_brazo__action__MoverBrazo_GetResult_Request__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<MoverBrazo_GetResult_Request>);
    fn control_brazo__action__MoverBrazo_GetResult_Request__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<MoverBrazo_GetResult_Request>, out_seq: *mut rosidl_runtime_rs::Sequence<MoverBrazo_GetResult_Request>) -> bool;
}

// Corresponds to control_brazo__action__MoverBrazo_GetResult_Request
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct MoverBrazo_GetResult_Request {

    // This member is not documented.
    #[allow(missing_docs)]
    pub goal_id: unique_identifier_msgs::msg::rmw::UUID,

}



impl Default for MoverBrazo_GetResult_Request {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !control_brazo__action__MoverBrazo_GetResult_Request__init(&mut msg as *mut _) {
        panic!("Call to control_brazo__action__MoverBrazo_GetResult_Request__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for MoverBrazo_GetResult_Request {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { control_brazo__action__MoverBrazo_GetResult_Request__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { control_brazo__action__MoverBrazo_GetResult_Request__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { control_brazo__action__MoverBrazo_GetResult_Request__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for MoverBrazo_GetResult_Request {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for MoverBrazo_GetResult_Request where Self: Sized {
  const TYPE_NAME: &'static str = "control_brazo/action/MoverBrazo_GetResult_Request";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__control_brazo__action__MoverBrazo_GetResult_Request() }
  }
}


#[link(name = "control_brazo__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__control_brazo__action__MoverBrazo_GetResult_Response() -> *const std::ffi::c_void;
}

#[link(name = "control_brazo__rosidl_generator_c")]
extern "C" {
    fn control_brazo__action__MoverBrazo_GetResult_Response__init(msg: *mut MoverBrazo_GetResult_Response) -> bool;
    fn control_brazo__action__MoverBrazo_GetResult_Response__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<MoverBrazo_GetResult_Response>, size: usize) -> bool;
    fn control_brazo__action__MoverBrazo_GetResult_Response__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<MoverBrazo_GetResult_Response>);
    fn control_brazo__action__MoverBrazo_GetResult_Response__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<MoverBrazo_GetResult_Response>, out_seq: *mut rosidl_runtime_rs::Sequence<MoverBrazo_GetResult_Response>) -> bool;
}

// Corresponds to control_brazo__action__MoverBrazo_GetResult_Response
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[allow(non_camel_case_types)]
#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct MoverBrazo_GetResult_Response {

    // This member is not documented.
    #[allow(missing_docs)]
    pub status: i8,


    // This member is not documented.
    #[allow(missing_docs)]
    pub result: super::super::action::rmw::MoverBrazo_Result,

}



impl Default for MoverBrazo_GetResult_Response {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !control_brazo__action__MoverBrazo_GetResult_Response__init(&mut msg as *mut _) {
        panic!("Call to control_brazo__action__MoverBrazo_GetResult_Response__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for MoverBrazo_GetResult_Response {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { control_brazo__action__MoverBrazo_GetResult_Response__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { control_brazo__action__MoverBrazo_GetResult_Response__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { control_brazo__action__MoverBrazo_GetResult_Response__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for MoverBrazo_GetResult_Response {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for MoverBrazo_GetResult_Response where Self: Sized {
  const TYPE_NAME: &'static str = "control_brazo/action/MoverBrazo_GetResult_Response";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__control_brazo__action__MoverBrazo_GetResult_Response() }
  }
}






#[link(name = "control_brazo__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_service_type_support_handle__control_brazo__action__MoverBrazo_SendGoal() -> *const std::ffi::c_void;
}

// Corresponds to control_brazo__action__MoverBrazo_SendGoal
#[allow(missing_docs, non_camel_case_types)]
pub struct MoverBrazo_SendGoal;

impl rosidl_runtime_rs::Service for MoverBrazo_SendGoal {
    type Request = MoverBrazo_SendGoal_Request;
    type Response = MoverBrazo_SendGoal_Response;

    fn get_type_support() -> *const std::ffi::c_void {
        // SAFETY: No preconditions for this function.
        unsafe { rosidl_typesupport_c__get_service_type_support_handle__control_brazo__action__MoverBrazo_SendGoal() }
    }
}




#[link(name = "control_brazo__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_service_type_support_handle__control_brazo__action__MoverBrazo_GetResult() -> *const std::ffi::c_void;
}

// Corresponds to control_brazo__action__MoverBrazo_GetResult
#[allow(missing_docs, non_camel_case_types)]
pub struct MoverBrazo_GetResult;

impl rosidl_runtime_rs::Service for MoverBrazo_GetResult {
    type Request = MoverBrazo_GetResult_Request;
    type Response = MoverBrazo_GetResult_Response;

    fn get_type_support() -> *const std::ffi::c_void {
        // SAFETY: No preconditions for this function.
        unsafe { rosidl_typesupport_c__get_service_type_support_handle__control_brazo__action__MoverBrazo_GetResult() }
    }
}


