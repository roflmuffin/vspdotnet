#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "core/script_engine.h"
#include <stack>

namespace vspdotnet
{
  extern std::stack<std::string> errors;
}

TEST_CASE("Script Engine Resetting", "[script_engine]")
{
  vspdotnet::fxNativeContext test{3, 1};
  auto value = vspdotnet::ScriptContextRaw(test);

  value.SetArgument(0, 50);
  REQUIRE(value.GetArgument<int>(0) == 50);

  value.SetResult(500);
  REQUIRE(value.GetResult<int>() == 500);

  SECTION("resetting vm clears the argument, result")
  {
    value.Reset();
    REQUIRE(value.GetArgument<int>(0) == 0);
    REQUIRE(value.GetResult<int>() == 0);
  }

  SECTION("resetting vm clears the error")
  {
    value.ThrowNativeError("Dummy Message");
    REQUIRE(value.HasError() == true);
    value.Reset();
    REQUIRE(value.HasError() == false);
    REQUIRE(value.GetResult<const char*>() == 0);
  }
}

TEST_CASE("Script Engine Limits", "[script_engine]") {
  vspdotnet::fxNativeContext test{32, 1};
  auto value = vspdotnet::ScriptContextRaw(test);

  SECTION("Can set all 32 arguments by index")
  {
    for (int i = 0; i < 32; i++) {
      value.SetArgument(i, i);
    }

    for (int i = 0; i < 32; i++) {
      REQUIRE(value.GetArgument<int>(i) == i);
    }
  }

  SECTION("Can set all 32 arguments by pushing")
  {
    value.Reset();

    for (int i = 0; i < 32; i++) {
      value.Push(i);
    }

    for (int i = 0; i < 32; i++) {
      REQUIRE(value.GetArgument<int>(i) == i);
    }
  }
}

TEST_CASE("Script Engine Supported Types", "[script_engine]")
{
  vspdotnet::fxNativeContext test{1, 1};
  auto value = vspdotnet::ScriptContextRaw(test);

  SECTION("Can set and get floats")
  {
    auto parameter_value = 100.0f;
    value.SetArgument(0, parameter_value);
    REQUIRE(value.GetArgument<float>(0) == parameter_value);
  }

  SECTION("Can set and get charPtr") {
    auto parameter_value = "Hello World";
    value.SetArgument(0, parameter_value);
    REQUIRE(value.GetArgument<const char*>(0) == parameter_value);
  }

  SECTION("Can set and get int") {
    auto parameter_value = 5;
    value.SetArgument(0, parameter_value);
    REQUIRE(value.GetArgument<int>(0) == parameter_value);
  }

  SECTION("Can set and get ulong") {
    auto parameter_value = ULONG_MAX;
    value.SetArgument(0, parameter_value);
    REQUIRE(value.GetArgument<unsigned long>(0) == parameter_value);
  }

  SECTION("Can set and get bool") {
    auto parameter_value = true;
    value.SetArgument(0, parameter_value);
    REQUIRE(value.GetArgument<unsigned long>(0) == parameter_value);
  }

  SECTION("Can set and get bool") {
    auto parameter_value = true;
    value.SetArgument(0, parameter_value);
    REQUIRE(value.GetArgument<unsigned long>(0) == parameter_value);
  }
}

TEST_CASE("Script Engine Errors", "[script_engine]")
{
  vspdotnet::fxNativeContext test{1, 1};
  auto value = vspdotnet::ScriptContextRaw(test);

  SECTION("Can throw simple text error message") {
    auto error_message = "This is a sample error message with no parameters.";
    value.ThrowNativeError(error_message);

    REQUIRE(value.HasError() == true);
    REQUIRE(strcmp(value.GetResult<const char*>(), error_message) == 0);

    value.Reset();
  }

  SECTION("Can throw error messages with interpolated variables")
  {
    auto error_message = "This is a sample error message with one parameter: %d.";
    value.ThrowNativeError(error_message, 100);

    REQUIRE(value.HasError() == true);
    REQUIRE(strcmp(value.GetResult<const char*>(), "This is a sample error message with one parameter: 100.") == 0);

    value.Reset();
  }

  SECTION("Error messages get garbage collected")
  {
    value.ThrowNativeError("Dummy Message");
    REQUIRE(vspdotnet::errors.size() == 1);
    value.Reset();
    REQUIRE(vspdotnet::errors.size() == 0);
  }
}

TEST_CASE("Script Engine Native Handlers", "[script_engine]")
{
  vspdotnet::fxNativeContext test{1, 1};
  auto value = vspdotnet::ScriptContextRaw(test);

  static bool callback_completed = false;
  static int returnValue = 100;

  vspdotnet::ScriptEngine::RegisterNativeHandler<int>(
    "TEST_NATIVE", [](vspdotnet::ScriptContext& script_context) {
        callback_completed = true;
        return returnValue;        
    });

  SECTION("Callback is registered successfully") {
    auto handler = vspdotnet::ScriptEngine::GetNativeHandler("TEST_NATIVE");
    handler.value()(value);

    REQUIRE(callback_completed == true);
  }

  SECTION("Callback can be called by the script engine")
  {
    value.Reset();
    callback_completed = false;

    test.nativeIdentifier = vspdotnet::hash_string("TEST_NATIVE");
    vspdotnet::ScriptEngine::InvokeNative(test);
    REQUIRE(callback_completed == true);
  }

  SECTION("Callback can return a value")
  {
    value.Reset();

    test.nativeIdentifier = vspdotnet::hash_string("TEST_NATIVE");
    vspdotnet::ScriptEngine::InvokeNative(test);

    REQUIRE(value.GetResult<int>() == returnValue);
  }
}