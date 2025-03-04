#pragma once


namespace rendering {
    class light {

    };

    namespace lights {

        class spot : public light {};
        class directional : public light{};
        class point : public light {};
    };
};