#include "resource.hpp"


using namespace pgreng::common;


/* Templates */
template <> const int& resource::deserializer<int>::operator()() {}
template <> const float& resource::deserializer<float>::operator()() {}
template <> const double& resource::deserializer<double>::operator()() {}
template <> const resource& resource::deserializer<resource>::operator()(){}