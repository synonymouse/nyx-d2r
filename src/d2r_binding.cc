#include "d2r_binding.h"

#include "d2r_methods.h"
#include "offsets.h"

#include <nyx/env.h>
#include <nyx/extension.h>
#include <nyx/isolate_data.h>
#include <nyx/util.h>

#include <dolos/pipe_log.h>

namespace d2r {

using nyx::Environment;
using v8::BigInt;
using v8::Context;
using v8::FunctionCallbackInfo;
using v8::HandleScope;
using v8::Isolate;
using v8::Local;
using v8::ObjectTemplate;
using v8::Value;

void RevealLevel(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  Environment* env = Environment::GetCurrent(isolate);
  Local<Context> context = env->context();
  if (!args[0]->IsUint32()) {
    return;
  }
  uint32_t level_id = args[0]->Uint32Value(context).FromJust();
  args.GetReturnValue().Set(RevealLevelById(level_id));
}

// will break on patch, look at the end of GetPlayerUnit for decryption method
static void GetPlayerIdByIndex(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  Environment* env = Environment::GetCurrent(isolate);
  Local<Context> context = env->context();
  if (!args[0]->IsUint32()) {
    return args.GetReturnValue().Set(-1);
  }
  uint32_t idx = args[0]->Uint32Value(context).FromJust();
  if (idx < 0 || idx >= 8) {
    return args.GetReturnValue().Set(-1);
  };

  uint32_t id = GetPlayerId(idx);
  args.GetReturnValue().Set(id);
}

static void GetLocalPlayerIndex(const FunctionCallbackInfo<Value>& args) {
  args.GetReturnValue().Set(*(uint32_t*)s_PlayerUnitIndex);
}

void GetClientSideUnitHashTableAddress(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  typedef void* (*GetClientSideUnitHashTableByTypeFn)(uint32_t);
  void* addr = reinterpret_cast<GetClientSideUnitHashTableByTypeFn>(GetClientSideUnitHashTableByType)(0);
  args.GetReturnValue().Set(BigInt::NewFromUnsigned(isolate, reinterpret_cast<uint64_t>(addr)));
}

void GetServerSideUnitHashTableAddress(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  typedef void* (*GetServerSideUnitHashTableByTypeFn)(uint32_t);
  void* addr = reinterpret_cast<GetServerSideUnitHashTableByTypeFn>(GetServerSideUnitHashTableByType)(0);
  args.GetReturnValue().Set(BigInt::NewFromUnsigned(isolate, reinterpret_cast<uint64_t>(addr)));
}

static void AddMarker(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  Environment* env = Environment::GetCurrent(isolate);
  Local<Context> context = env->context();
  if (args.Length() < 3 || !args[0]->IsInt32() || !args[1]->IsInt32() || !args[2]->IsUint32()) {
    return args.GetReturnValue().Set(false);
  }
  int32_t posX = args[0]->Int32Value(context).FromJust();
  int32_t posY = args[1]->Int32Value(context).FromJust();
  uint16_t cellNo = static_cast<uint16_t>(args[2]->Uint32Value(context).FromJust());
  args.GetReturnValue().Set(AddAutomapMarker(posX, posY, cellNo));
}

static void ClearMarkers(const FunctionCallbackInfo<Value>& args) {
  ClearAutomapMarkers();
}

static void TestCells(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();
  Environment* env = Environment::GetCurrent(isolate);
  Local<Context> context = env->context();
  if (args.Length() < 4 || !args[0]->IsInt32() || !args[1]->IsInt32() || !args[2]->IsUint32() ||
      !args[3]->IsUint32()) {
    return args.GetReturnValue().Set(false);
  }
  int32_t baseX = args[0]->Int32Value(context).FromJust();
  int32_t baseY = args[1]->Int32Value(context).FromJust();
  uint16_t startId = static_cast<uint16_t>(args[2]->Uint32Value(context).FromJust());
  uint16_t count = static_cast<uint16_t>(args[3]->Uint32Value(context).FromJust());
  args.GetReturnValue().Set(TestAutomapCells(baseX, baseY, startId, count));
}

void InitD2RBinding(nyx::IsolateData* isolate_data, Local<ObjectTemplate> target) {
  Isolate* isolate = isolate_data->isolate();

  nyx::SetMethod(isolate, target, "log", [](const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    HandleScope handle_scope(isolate);
    nyx::Utf8Value utf8(isolate, args[0]);
    PIPE_LOG(*utf8);
  });

  nyx::SetMethod(isolate, target, "revealLevel", RevealLevel);
  nyx::SetMethod(isolate, target, "getPlayerIdByIndex", GetPlayerIdByIndex);
  nyx::SetMethod(isolate, target, "getLocalPlayerIndex", GetLocalPlayerIndex);
  nyx::SetMethod(isolate, target, "getClientSideUnitHashTableAddress", GetClientSideUnitHashTableAddress);
  nyx::SetMethod(isolate, target, "getServerSideUnitHashTableAddress", GetServerSideUnitHashTableAddress);
  nyx::SetMethod(isolate, target, "addAutomapMarker", AddMarker);
  nyx::SetMethod(isolate, target, "clearAutomapMarkers", ClearMarkers);
  nyx::SetMethod(isolate, target, "testAutomapCells", TestCells);
}

}  // namespace d2r
