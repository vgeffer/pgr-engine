#pragma once
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include "../rendering/mesh.hpp"

namespace physics {

    enum bounding_volume_type {
        SPHERE, OBB, SURFACE
    };

    class bounding_volume {

        public:
            virtual bool intersects(bounding_volume& other);
            virtual void transform(glm::mat4x4& mat);

            inline bounding_volume_type type() const { return _type; }

        protected:
            bounding_volume_type _type;
            explicit bounding_volume(const bounding_volume_type type);

    };

    namespace bounding_volumes {

        class sphere : public bounding_volume {

            public:
                sphere(glm::vec3& center, float radius);

                bool intersects(bounding_volume& other) override;
                void transform(glm::mat4x4& mat) override;

                inline float radius() const { return _radius; };
                inline glm::vec3 center() const { return _center; };

            private:
                float _radius;
                glm::vec3 _center;
        };

        class obb : public bounding_volume {

            bool intersects(bounding_volume& other) override;
            void transform(glm::mat4x4& mat) override;

        };

        class surface : public bounding_volume {

            bool intersects(bounding_volume& other) override;
            void transform(glm::mat4x4& mat) override;

            private:
                rendering::mesh& mesh;
        };
    };

};