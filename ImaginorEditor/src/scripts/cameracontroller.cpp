#include <ImaginorEngine/src/script.h>
#include <ImaginorMath/src/LinearMath/linearmath.h>
#include <ImaginorEngine/src/components.h>

namespace IME {
    class cameracontroller
    {
    private:
        /* data */
        EulerTransform transform;

        vec2<int32> lastmousepos;

    public:
        Scene::Entity self;

        void onInit(IME::Scene::SceneData* scene) {
            Scene::TransformComponent* transform = Scene::getComponent<Scene::TransformComponent>(self, scene);
            this->transform = eulerTransformFromMat4(transform->transform);
        }

        void onEvent(IME::Event e, IME::Scene::SceneData* scene, const IME::PlatformInterface& platform) {
            if(e.type == IME_MOUSE_BUTTON_PRESSED) {
                lastmousepos = platform.mouse.relativemousepos;
            }
        }

        void onUpdate(IME::Scene::SceneData* scene, const IME::PlatformInterface& platform) {
            real32 delta = platform.time.lastframetime;
            real32 v = 5.0f;

            vec3f direction = {0.0f, 0.0f, 0.0f};

            if(platform.mouse.isButtonPressed(IME_RIGHT_MB)) {
                vec2<int32> deltamousepos = lastmousepos - platform.mouse.relativemousepos;
                real32 sensitivity = 0.5f;

                this->transform.roll += sensitivity * deltamousepos.x;
                this->transform.pitch += sensitivity * deltamousepos.y;
            }

            if(platform.keyboard.isKeyPressed(IME_KEY_A)) {
                direction.x += -v * delta;
            }
            if(platform.keyboard.isKeyPressed(IME_KEY_D)) {
                direction.x += v * delta;
            }
            if(platform.keyboard.isKeyPressed(IME_KEY_W)) {
                direction.z += -v * delta;
            }
            if(platform.keyboard.isKeyPressed(IME_KEY_S)) {
                direction.z += +v * delta;
            }

            lastmousepos = platform.mouse.relativemousepos;

            mat3 rotation = rotationMat3FromEulerAngles(this->transform.rotation);
            direction = rotation * direction;
            this->transform.translation += direction;
            vec3f t = this->transform.translation;

            Scene::TransformComponent* transform = Scene::getComponent<Scene::TransformComponent>(self, scene);
            transform->transform = rowComposeMat4(toVec4(rotation.rows[0], t.x), toVec4(rotation.rows[1], t.y), toVec4(rotation.rows[2], t.z), {0.0f, 0.0f, 0.0f, 1.0f});
        }
    };

    ALLOC_SCRIPT_DATA(alloc_cameracontroller) {
        return Memory::alloc(sizeof(cameracontroller));
    }
    DEALLOC_SCRIPT_DATA(dealloc_cameracontroller) {
        Memory::dealloc(sizeof(cameracontroller), (IME::byte*)data);
    }
    SCRIPT_ON_INIT(cameracontroller_on_init) {
        ((cameracontroller*)script_data)->self = entity;
        ((cameracontroller*)script_data)->onInit(scene);
    }
    SCRIPT_ON_UPDATE(cameracontroller_on_update) {
        ((cameracontroller*)script_data)->onUpdate(scene, platform);
    }
    SCRIPT_ON_EVENT(cameracontroller_on_update) {
        ((cameracontroller*)script_data)->onEvent(e, scene, platform);
    }

}

