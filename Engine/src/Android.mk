LOCAL_PATH := $(call my-dir)
$(info Building Ossium module. LOCAL_PATH=$(LOCAL_PATH))

include $(CLEAR_VARS)

LOCAL_MODULE := Ossium

SDL_PATH := ../SDL

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include $(LOCAL_PATH)/../SDL2_image/ $(LOCAL_PATH)/../SDL2_ttf/ $(LOCAL_PATH)/../SDL2_mixer/ $(LOCAL_PATH)/../SDL2_net/ $(LOCAL_PATH)/../Box2D

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)

# Add your application source files here...
LOCAL_SRC_FILES := Editor/main.cpp Editor/Core/editorserializer.cpp $(subst $(LOCAL_PATH)/,, \
  	$(wildcard $(LOCAL_PATH)/Core/*.cpp) \
  	$(wildcard $(LOCAL_PATH)/Components/*.cpp) \
  	$(wildcard $(LOCAL_PATH)/Components/UI/*.cpp))

# Box2D and other static libs
LOCAL_STATIC_LIBRARIES := Box2D

# SDL2 and extension libraries
LOCAL_SHARED_LIBRARIES := SDL2 SDL2_image SDL2_ttf SDL2_mixer SDL2_net

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -llog

include $(BUILD_STATIC_LIBRARY)