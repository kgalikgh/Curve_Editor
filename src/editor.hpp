#ifndef EDITOR_HPP
#define EDITOR_HPP
enum EditorMode {Add, Move};

class Editor 
{
  Editor();
  void setEditorMode(EditorMode);
  EditorMode getEditorMode();
  void setActiveCurve(Curve*);
  Curve* getActiveCurve();
  
  private:
  EditorMode currentMode;
  Curve* activeCurve;
}
#endif
