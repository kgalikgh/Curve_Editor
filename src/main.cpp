#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <string>
#include <cmath>
#include <map>

#include "constants.hpp"
#include "curve.hpp"
#include "point.hpp"

enum EditorMode { None, AddRmNodes, MoveNodes, MoveCurve, RotateCurve};
static std::map<EditorMode, std::string> modeStr{
  {EditorMode::None, "None"},
  {EditorMode::AddRmNodes, "Add/Remove nodes"},
  {EditorMode::MoveNodes, "Move nodes"},
  {EditorMode::MoveCurve, "Move curve"},
  {EditorMode::RotateCurve, "Rotate curve"}
};

Curve* activeCurve = nullptr;
Node* activePoint = nullptr;
sf::Vector2f lastPos;
sf::Vector2f rotationPoint = {0,0};
static EditorMode currentMode = EditorMode::None;
static int curvesNum = 0;

void moveNode(float x, float y)
{
  if((activePoint == nullptr) || (activeCurve == nullptr)) return;
  activePoint->setPosition(x, y);
  activeCurve->updateCurve();
} 

void moveCurve(float x, float y)
{
  if((activePoint == nullptr) || (activeCurve == nullptr)) return;
  auto x_offset = x - lastPos.x;
  auto y_offset = y - lastPos.y;
  activeCurve->moveNodes(x_offset, y_offset);
  lastPos = sf::Vector2f(x,y); 
}

void rotateCurve(float x, float y, int window_width)
{
  if((activeCurve == nullptr) || (rotationPoint.x == 0 && rotationPoint.y == 0)) return;
  //[0-100] -> [0-2pi]
  double ratio = 2000.0;
  float angle = (x - rotationPoint.x) * 2.0 * M_PI / ratio;
  std::cout<<angle<<std::endl;
  if(angle >= 0.00001 || angle <= -0.00001)
    activeCurve->rotateNodes(rotationPoint, angle); 
}

void selectRotationPoint(tgui::Vector2f pos)
{
  if(currentMode != EditorMode::RotateCurve) return;
  rotationPoint = pos;
}

void deselectRotationPoint()
{
  rotationPoint = sf::Vector2f();
}

// Callbacks
void addPoint(tgui::Vector2f pos)
{
  if(currentMode != EditorMode::AddRmNodes) return;
  if(activeCurve == nullptr) return;
  Node node(pos);
  activeCurve->addNode(node);
}

void removePoint(tgui::Vector2f pos)
{
  if(currentMode != EditorMode::AddRmNodes) return;
  if(activeCurve == nullptr) return;
  activeCurve->removeClickedNode(pos); 
}

void selectPoint(tgui::Vector2f pos)
{
  if((currentMode != EditorMode::MoveNodes) && (currentMode != EditorMode::MoveCurve)) return;
  if(activePoint != nullptr) return;
  activePoint = activeCurve->findClickedNode(pos);
  if(activePoint)
  {
    lastPos = pos;
    activePoint->select();
  }
}

void deselectAllPoints()
{
  if((currentMode != EditorMode::MoveNodes) && (currentMode != EditorMode::MoveCurve)) return;
  if(activePoint == nullptr) return;
  activePoint->deselect();
  activePoint = nullptr;
}

void addCurve(std::vector<Curve>& curves, tgui::ListBox::Ptr listBox)
{
  std::string s = "Curve " + std::to_string(curvesNum);
  curvesNum++;
  listBox->addItem(s);
  listBox->setSelectedItemByIndex(-1);
  currentMode = EditorMode::None;
  Curve c;
  curves.push_back(c);
  for(auto& curve : curves)
    curve.deselect();
  activeCurve = nullptr;
} 

void removeCurve(std::vector<Curve>& curves, tgui::ListBox::Ptr listBox)
{
  activeCurve = nullptr;
  int index = listBox->getSelectedItemIndex();
  curves.erase(curves.begin() + index);
  listBox->removeItemByIndex(index);
  listBox->setSelectedItemByIndex(-1);
  currentMode = EditorMode::None;
} 

void selectCurve(std::vector<Curve>& curves, tgui::Group::Ptr curvePropsGroup, int index)
{
  if(curves.empty()) return;
  if(index > curves.size()) return; 
  auto curveType = curvePropsGroup->get<tgui::ComboBox>("CurvesTypeComboBox");
  auto stepSlider = curvePropsGroup->get<tgui::Slider>("PrecissionSlider");
  auto sliderValue = curvePropsGroup->get<tgui::Label>("SliderValue");
  if(activeCurve)
  {
    activeCurve->deselect();
  }
  activeCurve = &curves[index];
  activeCurve->select();
  curveType->setSelectedItemByIndex((int)activeCurve->getCurveType());
  stepSlider->setValue(activeCurve->getStep());
  std::string s = std::to_string(activeCurve->getStep());
  sliderValue->setText(s);
}

void switchMode(EditorMode mode, tgui::Label::Ptr modeText)
{
  currentMode = mode;
  std::string str = modeStr[mode];
  modeText->setText(str);
}

void pickCurveType(int index)
{
  activeCurve->changeCurveType((CurveType)index);
}

void updateCurveStep(tgui::Label::Ptr sliderVal, float newVal)
{
  if(activeCurve == nullptr) return;
  std::string str = std::to_string(newVal);
  sliderVal->setText(str);
  activeCurve->setStep(newVal);
}

int main()
{
  std::vector<Curve> curves; 

  // Setup window
  sf::RenderWindow window{{1366, 768}, "TGUI example - SFML_GRAPHICS backend"};
  tgui::Gui gui{window};
  gui.loadWidgetsFromFile("gui/editor_gui.txt"); 

  auto canvas = tgui::CanvasSFML::create();
  canvas->setPosition({"20%","0%"});
  canvas->setSize({"100%", "100%"});
  gui.add(canvas);

  auto curvesListBox = gui.get<tgui::ListBox>("CurvesListBox");
  auto addButton = gui.get<tgui::Button>("AddCurveBtn");
  auto deleteButton = gui.get<tgui::Button>("DeleteCurveBtn");
  auto buttonsGroup = gui.get<tgui::Group>("ButtonsGroup");
  auto addRemoveNodesButton = gui.get<tgui::Button>("AddRmNodesBtn");
  auto moveNodesButton = gui.get<tgui::Button>("MoveNodesBtn");
  auto moveCurveButton = gui.get<tgui::Button>("MoveCurveBtn");
  auto rotateCurveButton = gui.get<tgui::Button>("RotateCurveBtn");
  auto modeText = gui.get<tgui::Label>("ModeText");
  auto curveTypesComboBox = gui.get<tgui::ComboBox>("CurvesTypeComboBox");
  auto curveOptions = gui.get<tgui::Group>("CurveOptions");
  auto stepSlider = gui.get<tgui::Slider>("PrecissionSlider");
  auto stepValueLabel = gui.get<tgui::Label>("SliderValue");

  addButton->onPress(&addCurve, std::ref(curves), curvesListBox);
  deleteButton->onPress(&removeCurve, std::ref(curves), curvesListBox);
  addRemoveNodesButton->onPress(&switchMode, EditorMode::AddRmNodes, modeText);
  moveNodesButton->onPress(&switchMode, EditorMode::MoveNodes, modeText);
  moveCurveButton->onPress(&switchMode, EditorMode::MoveCurve, modeText);
  rotateCurveButton->onPress(&switchMode, EditorMode::RotateCurve, modeText);
  curveTypesComboBox->onItemSelect(&pickCurveType);
  curveTypesComboBox->addItem("Polyline");
  curveTypesComboBox->addItem("LagrangeInterpolation");
  curveTypesComboBox->addItem("Bezier");
  curvesListBox->onItemSelect(&selectCurve, std::ref(curves), curveOptions);
  stepSlider->onValueChange(&updateCurveStep, stepValueLabel);
  
  canvas->onMousePress(&addPoint);
  canvas->onMousePress(&selectPoint);
  canvas->onMousePress(&selectRotationPoint);
  canvas->onMouseRelease(&deselectAllPoints);
  canvas->onMouseRelease(&deselectRotationPoint);
  canvas->onRightMousePress(&removePoint);
 
  // Main loop
  while (window.isOpen())
  {
    sf::Event event;
    while (window.pollEvent(event))
    {
      gui.handleEvent(event);
  
      if (event.type == sf::Event::Closed)
      {
        window.close();
      }

      //Handle custom canvas events
      switch(currentMode)
      {
        case EditorMode::MoveNodes:
        if (event.type == sf::Event::MouseMoved)
        {
          moveNode(event.mouseMove.x - 0.2 * window.getSize().x, event.mouseMove.y);
        }
        break;
        case EditorMode::MoveCurve:
        if (event.type == sf::Event::MouseMoved)
        {
          moveCurve(event.mouseMove.x - 0.2 * window.getSize().x, event.mouseMove.y);
        }
        break;
        case EditorMode::RotateCurve:
        if (event.type == sf::Event::MouseMoved)
        {
          rotateCurve(event.mouseMove.x - 0.2 * window.getSize().x, event.mouseMove.y, window.getSize().x);
        }
        break;
      }
    }

    // Set conditionally visible elements
    deleteButton->setEnabled(curvesListBox->getSelectedItemIndex() != -1);
    curveOptions->setVisible(activeCurve != nullptr);

    window.clear();
    canvas->clear(drawingBackgroundColor);
    
    for(auto const& curve : curves) 
    {
      canvas->draw(curve);
    }
    canvas->display();
    gui.draw();
  
    window.display();
  }
}

