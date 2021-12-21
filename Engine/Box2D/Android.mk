LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := Box2D

LOCAL_SRC_FILES := $(BOX2D_CPP) Box2D/Dynamics/b2World.cpp $(subst $(LOCAL_PATH)/,, \
                    $(wildcard $(LOCAL_PATH)/Box2D/Collision/*.cpp) \
                    $(wildcard $(LOCAL_PATH)/Box2D/Collision/Shapes/*.cpp) \
                    $(wildcard $(LOCAL_PATH)/Box2D/Common/*.cpp) \
                    $(wildcard $(LOCAL_PATH)/Box2D/Dynamics/*.cpp) \
                    $(wildcard $(LOCAL_PATH)/Box2D/Dynamics/Joints/*.cpp) \
                    $(wildcard $(LOCAL_PATH)/Box2D/Dynamics/Contacts/*.cpp) \
                    $(wildcard $(LOCAL_PATH)/Box2D/Rope/*.cpp))

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/Box2D

LOCAL_C_INCLUDES := $(LOCAL_EXPORT_C_INCLUDES)

include $(BUILD_STATIC_LIBRARY)