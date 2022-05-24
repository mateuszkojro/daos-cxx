#include "Pool.h"
#include "Errors.h"
#include "daos_types.h"
#include <cassert>
#include <cstddef>
#include <memory>
#include <stdexcept>

Pool::Pool(UUID pool_uuid) { connect(pool_uuid.raw()); }

Pool::Pool(const std::string pool_label) { connect(pool_label.c_str()); }

// TODO: Closing all the connections
Pool::~Pool() { clean_up(); }

void Pool::connect(const std::string& label) {
  unsigned flags = DAOS_PC_RW;
  DAOS_CHECK(
	  daos_pool_connect(label.c_str(), NULL, flags, &pool_handle_, NULL, NULL));
  if (daos_handle_is_inval(pool_handle_)) {
	throw std::runtime_error("Handle is invalid in Pool:connect()");
  }
}

void Pool::connect(const unsigned char* uuid) {
  unsigned flags = DAOS_PC_RW;
  DAOS_CHECK(daos_pool_connect(reinterpret_cast<const char*>(uuid), NULL, flags,
							   &pool_handle_, NULL, NULL));
  if (daos_handle_is_inval(pool_handle_)) {
	throw std::runtime_error("Handle is invalid in Pool:connect()");
  }
}

void Pool::clean_up() { DAOS_CHECK(daos_pool_disconnect(pool_handle_, NULL)); }

ContainerPtr Pool::add_container(const std::string& name) {
  // daos_prop_t cont_prop;
  uuid_t container_uuid;

  if (name.empty()) {
	MK_UNIMPLEMENTED;
	DAOS_CHECK(daos_cont_create_cpp(pool_handle_, &container_uuid, NULL, NULL));
  } else {
	DAOS_CHECK(daos_cont_create_with_label(pool_handle_, name.c_str(), NULL,
										   &container_uuid, NULL));
  }

  return std::make_unique<Container>(name, pool_handle_);
}

// TODO: not tested!
ContainerPtr Pool::get_container(const UUID& uuid) {
  daos_handle_t container_handle;
  int flags = 0;
  DAOS_CHECK(daos_cont_open(pool_handle_, uuid.raw(), flags, &container_handle,
							NULL, NULL));
  return std::make_unique<Container>(container_handle);
}

void Pool::remove_container(const std::string& container_label) {
  bool force_destroy = true;
  DAOS_CHECK(daos_cont_destroy(pool_handle_, container_label.c_str(),
							   force_destroy, NULL));
}

void Pool::remove_container(ContainerPtr& ptr) {
  MK_UNIMPLEMENTED;
  // bool force_destroy = true;
  // FIXME: remove this hack
  //  DAOS_CHECK(daos_cont_destroy(
  //   pool_handle_, ptr->get_uuid().raw(), force_destroy, NULL));
}

std::unique_ptr<daos_handle_t> Pool::get_pool_handle() {
  return std::make_unique<daos_handle_t>(pool_handle_);
}
