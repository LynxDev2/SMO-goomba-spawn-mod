#include "patches.hpp"
#include "al/PlayerHolder/PlayerHolder.h"
#include "al/factory/ProjectCameraPoserFactory.h"
#include "al/fs/FileLoader.h"
#include "armv8/instructions/op101x/unconditional_branch_immediate/b.hpp"
#include "armv8/instructions/op101x/unconditional_branch_immediate/bl.hpp"
#include "filedevice/nin/seadNinSDFileDeviceNin.h"
#include "filedevice/seadFileDevice.h"
#include "filedevice/seadFileDeviceMgr.h"
#include "filedevice/seadPath.h"
#include "fs/fs_mount.hpp"
#include "game/StageScene/StageScene.h"
#include "logger/Logger.hpp"
#include "resource/seadArchiveRes.h"

namespace patch = exl::patch;
namespace inst = exl::armv8::inst;
namespace reg = exl::armv8::reg;

al::PlayerHolder *createTicket(StageScene *curScene);

void costumeRoomPatches() {
  patch::CodePatcher p(0x262850);
  p.WriteInst(inst::Movz(reg::W0, 0));
  p.Seek(0x2609B4);
  p.WriteInst(inst::Movz(reg::W0, 0));

  p.Seek(0x25FF74);
  p.WriteInst(inst::Movz(reg::W0, 1));
  p.Seek(0x25FF74);
  p.WriteInst(inst::Movz(reg::W0, 0));
}

void cameraPatches() {
  patch::CodePatcher p(0x4BE628);
  p.WriteInst(inst::Branch((void *)ProjectCameraPoserFactory));
  p.Seek(0x4C9080);
  p.WriteInst(inst::BranchLink((void *)createTicket));
}

void stubSocketInit() {
  patch::CodePatcher p(0x95C498);
  p.WriteInst(inst::Nop());
}

void runCodePatches() {
  // costumeRoomPatches();
  // stubSocketInit();
  cameraPatches();
}

void installCommonHooks() {
  // SD File Redirection

  //    RedirectFileDevice::InstallAtOffset(0x76CFE0);
  //  FileLoaderLoadArc::InstallAtOffset(0xA5EF64);
  // CreateFileDeviceMgr::InstallAtOffset(0x76C8D4);
  // FileLoaderIsExistFile::InstallAtOffset(0xA5ED28);

  // Sead Debugging Overriding
}

al::PlayerHolder *createTicket(StageScene *curScene) {
  // only creates custom gravity camera ticket if hide and seek mode is
  // active
  al::CameraDirector *director = curScene->getCameraDirector();
  if (director) {
    if (director->mFactory) {
      al::CameraTicket *customCamera = director->createCameraFromFactory(
          "CameraPoserCustom", nullptr, 0, 5, sead::Matrix34f::ident);

      if (customCamera) {
        al::startCamera(curScene, customCamera, -1);
      }
    }
  }
  return al::getScenePlayerHolder(curScene);
}
