#pragma once

#include "CameraTicket.h"

namespace al {
class IUseCamera;
class PlacementInfo;
class CameraPoser;
} // namespace al

class alCameraFunction {
public:
  static void initCameraNoSave(al::CameraPoser *, al::IUseCamera const *,
                               al::PlacementInfo const &, char const *, int);
  static void initPriorityDemo2(al::CameraTicket *);
  static void validateKeepPreSelfPoseNextCamera(al::CameraTicket *);
  static void validateCameraInterpoleEaseOut(al::CameraTicket *);
};
