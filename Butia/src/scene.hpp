#pragma once

class Scene {
    public:
    void static runtime();
    void static update();
    void static once();
    void static input();
    void static ui();
    void static post_processing();
    void static newframe();
    void static draw();
};
