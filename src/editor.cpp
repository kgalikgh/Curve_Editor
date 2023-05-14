#include "editor.hpp"

Editor::Editor() : activeCurve(nullptr), currentMode(EditorMode::Add) {}

void Editor::setEditorMode(EditorMode newMode)
{
  currentMode = newMode;
}

EditorMode getEditorMode()
{
  return currentMode;
}

void Editor::setActiveCurve(Curve* newCurve)
{
  activeCurve = newCurve;
}

Curve* Editor::getActiveCurve()
{
  return activeCurve;
}
