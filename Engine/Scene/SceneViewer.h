//
// Created by Kij on 25/06/18.
//

#ifndef SCENEVIEWER_H
#define SCENEVIEWER_H

#include "../Utils/InputManager.h"
#include "../Primitives/Empty.h"

enum class EViewerState {
    VIEWER_EDIT,
    VIEWER_PLACE,
    VIEWER_MOVE
};

class SceneViewer : IInputResponse, Empty {
public:
    SceneViewer();

    void SetState(EViewerState newState);

    void Begin() override;

private:
    void RespondToButton(int key, int button, int scancode, int action, int mods) override;
    void RespondToCursor(double xPos, double yPos) override;

    EViewerState ViewerState;

};



#endif //SCENEVIEWER_H
