#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "core/callback_manager.h"

namespace vspdotnet {
  vspdotnet::GlobalClass* vspdotnet::GlobalClass::head = nullptr;

    TEST_CASE("Can create callback", "[callbacks]") {
    vspdotnet::CallbackManager callback_manager;

    auto cb = callback_manager.CreateCallback("my_callback");
    REQUIRE(cb != nullptr);
  }

  static bool invoked = false;
  void __stdcall MyListener(fxNativeContext* native_context)
  { invoked = true;
  }

  TEST_CASE("Function listeners", "[callbacks]") {
    vspdotnet::CallbackManager callback_manager;

    auto cb = callback_manager.CreateCallback("my_callback");
    REQUIRE(cb != nullptr);

    SECTION("Name is correct") {
      REQUIRE(cb->GetName().compare("my_callback") == 0);
    }

    SECTION("Adding lambda listeners increase function count")
    {
      auto lambda = [](fxNativeContext* native_context) {

      };
      cb->AddListener(lambda);

      REQUIRE(cb->GetFunctionCount() == 1);
    }

    SECTION("Removing lambda listeners decreases function count") {
      auto lambda = [](fxNativeContext* native_context) {

      };

      cb->AddListener(lambda);
      REQUIRE(cb->GetFunctionCount() == 1);

      cb->RemoveListener(lambda);
      REQUIRE(cb->GetFunctionCount() == 0);
    }

    SECTION("Adding function listeners increases function count") {
      cb->AddListener(&MyListener);
      REQUIRE(cb->GetFunctionCount() == 1);
    }

    SECTION("Removing function listeners decreases function count") {
      cb->AddListener(&MyListener);
      REQUIRE(cb->GetFunctionCount() == 1);

       cb->RemoveListener(&MyListener);
      REQUIRE(cb->GetFunctionCount() == 0);
    }

    SECTION("Lambda listeners get called")
    {
      invoked = false;
      auto lambda = [](fxNativeContext* native_context) {
        invoked = true;
      };
      cb->AddListener(lambda);
      cb->Execute();

      REQUIRE(invoked == true);
    }

    SECTION("Function listeners get called") {
      invoked = false;
      cb->AddListener(&MyListener);
      cb->Execute();

      REQUIRE(invoked == true);
    }
  }

  TEST_CASE("Callback lists", "[callbacks]")
  {
    vspdotnet::CallbackManager callback_manager;

    auto cb = callback_manager.CreateCallback("my_callback");
    REQUIRE(cb != nullptr);

    SECTION("Can find callback by name")
    {
      auto my_found_cb = callback_manager.FindCallback("my_callback");
      REQUIRE(my_found_cb != nullptr);
    }

    SECTION("Can delete callback") {
      auto my_found_cb = callback_manager.FindCallback("my_callback");
      callback_manager.ReleaseCallback(my_found_cb);

      my_found_cb = callback_manager.FindCallback("my_callback");

      REQUIRE(my_found_cb == nullptr);
    }
  }


}