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
#include "tinyfiledialogs.h"

enum EditorMode { None, AddRmNodes, MoveNodes, MoveCurve, RotateCurve, DivideBezier};
static std::map<EditorMode, std::string> modeStr{
  {EditorMode::None, "None"},
  {EditorMode::AddRmNodes, "Add/Remove nodes"},
  {EditorMode::MoveNodes, "Move nodes"},
  {EditorMode::MoveCurve, "Move curve"},
  {EditorMode::RotateCurve, "Rotate curve"},
  {EditorMode::DivideBezier, "Divide Bezier curve"}
};

std::shared_ptr<Curve> activeCurve;
Node* activePoint = nullptr;
sf::Vector2f lastPos;
sf::Vector2f rotationPoint = {0,0};
static EditorMode currentMode = EditorMode::None;
static int curvesNum = 0;
static float lastAngle = 0.0f;
static const float indicatorRadius = 5.0;
static sf::CircleShape divideIndicator(indicatorRadius, 30);
static sf::CircleShape rotationPointSprite(indicatorRadius, 30);
static sf::Texture* backgroundTexture=nullptr;

void moveNode(float x, float y)
{
  if((activePoint == nullptr) || (!activeCurve)) return;
  activePoint->setPosition(x, y);
  activeCurve->updateCurve();
} 

void moveCurve(float x, float y)
{
  if((activePoint == nullptr) || (!activeCurve)) return;
  auto x_offset = x - lastPos.x;
  auto y_offset = y - lastPos.y;
  activeCurve->moveNodes(x_offset, y_offset);
  lastPos = sf::Vector2f(x,y); 
}

void rotateCurve(float x, float y, int window_width)
{
  if((!activeCurve) || (rotationPoint.x == 0 && rotationPoint.y == 0)) return;
  //[0-100] -> [0-2pi]
  double ratio = 500.0;
  float angle = (x - rotationPoint.x) * 2.0 * M_PI / ratio;
  activeCurve->rotateNodes(rotationPoint, lastAngle - angle); 
  lastAngle = angle;
}

void selectRotationPoint(tgui::Vector2f pos)
{
  if(currentMode != EditorMode::RotateCurve) return;
  lastAngle = 0.0f;
  rotationPoint = pos;
  rotationPointSprite.setPosition(rotationPoint);
}

void deselectRotationPoint()
{
  if(currentMode != EditorMode::RotateCurve) return;
  rotationPoint = sf::Vector2f();
}

// Callbacks
void addPoint(tgui::Vector2f pos)
{
  if(currentMode != EditorMode::AddRmNodes) return;
  if(!activeCurve) return;
  Node node(pos);
  activeCurve->addNode(node);
}

void removePoint(tgui::Vector2f pos)
{
  if(currentMode != EditorMode::AddRmNodes) return;
  if(!activeCurve) return;
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

void addCurve(std::vector<std::shared_ptr<Curve>>& curves, tgui::ListBox::Ptr listBox)
{
  std::string s = "Curve " + std::to_string(curvesNum);
  curvesNum++;
  listBox->addItem(s);
  listBox->setSelectedItemByIndex(-1);
  currentMode = EditorMode::None;
  curves.push_back(std::make_shared<Curve>());
  for(auto& curve : curves)
    curve->deselect();
  activeCurve.reset();
} 

void removeCurve(std::vector<std::shared_ptr<Curve>>& curves, tgui::ListBox::Ptr listBox)
{
  activeCurve.reset();
  int index = listBox->getSelectedItemIndex();
  curves[index].reset();
  curves.erase(curves.begin() + index);
  listBox->removeItemByIndex(index);
  listBox->setSelectedItemByIndex(-1);
  currentMode = EditorMode::None;
} 

void selectCurve(std::vector<std::shared_ptr<Curve>>& curves, tgui::Group::Ptr curvePropsGroup, int index)
{
  if(curves.empty()) return;
  if(index > curves.size()) return; 
  auto curveType = curvePropsGroup->get<tgui::ComboBox>("CurvesTypeComboBox");
  auto thicknessSlider = curvePropsGroup->get<tgui::Slider>("ThicknessSlider");
  if(activeCurve)
  {
    activeCurve->deselect();
  }
  activeCurve = curves[index];
  activeCurve->select();
  int a = (int)activeCurve->getCurveType();
  curveType->setSelectedItemByIndex(a);
  thicknessSlider->setValue(activeCurve->getThickness());
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

void updateCurveThickness(float newVal)
{
  activeCurve->setThickness(newVal);
}

void elevateBezier()
{
  if(!activeCurve) return;
  activeCurve->elevate();
}

void reduceBezier()
{
  if(!activeCurve) return;
  activeCurve->deelevate();
}

void divideBezier(std::vector<std::shared_ptr<Curve>>& curves, tgui::ListBox::Ptr listBox, tgui::Slider::Ptr divideSlider)
{
  if(!activeCurve) return;
  float f = divideSlider->getValue();
  if(f == 0.0 || f == 1.0) return;
  std::vector<Node> c1;
  std::vector<Node> c2;
  activeCurve->divide(f, c1, c2);
  removeCurve(curves, listBox);
  addCurve(curves, listBox);
  addCurve(curves, listBox);
  int n = curves.size();
  std::shared_ptr<Curve> cur1 = curves[n-2];
  std::shared_ptr<Curve> cur2 = curves[n-1];
  for(auto& node : c1)
  {
    cur1->addNode(node); 
  }
  for(auto& node : c2)
  {
    cur2->addNode(node);
  }
  cur1->changeCurveType(CurveType::Bezier);
  cur2->changeCurveType(CurveType::Bezier);
}

void loadBackground()
{
  char const *  lFilterPatterns[3] = {"*.jpg", "*.png", "*.jpeg"};
  auto path = tinyfd_openFileDialog ("Open Image","/home/kalej",3,lFilterPatterns,"image files",0);
  if(path ==  NULL) return;
  sf::Image img;
  if(img.loadFromFile(std::string(path)))
  {
    backgroundTexture = new sf::Texture();
    if(!backgroundTexture->loadFromImage(img))
    {
      delete backgroundTexture;
      backgroundTexture = nullptr;
    }
  }
}

void clearBackground()
{
  if(backgroundTexture == nullptr) return;
  delete backgroundTexture;
  backgroundTexture = nullptr;
}

int main()
{
  std::vector<std::shared_ptr<Curve>> curves; 

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
  auto thicknessSlider = gui.get<tgui::Slider>("ThicknessSlider");
  
  auto bezierOptionsGroup = gui.get<tgui::Group>("BezierOptions");
  auto divideBezierGroup = gui.get<tgui::Group>("DivideBezierGroup");

  auto elevateButton = gui.get<tgui::Button>("ElevateDegreeBtn");
  auto reduceButton = gui.get<tgui::Button>("ReduceDegreeBtn");
  auto divideModeButton = gui.get<tgui::Button>("DivideModeBtn");

  auto divisionPointsSlider = gui.get<tgui::Slider>("DivisionPointSlider");
  auto divideButton = gui.get<tgui::Button>("DivideBtn");

  auto clrBackgroundBtn = gui.get<tgui::Button>("ClearBackgroundBtn");
  auto loadBackgroundBtn = gui.get<tgui::Button>("LoadBackgroundBtn");

  thicknessSlider->setMinimum(thickMin);
  thicknessSlider->setMaximum(thickMax);
  thicknessSlider->setStep(thickStep);

  divisionPointsSlider->setMinimum(divisionMin);
  divisionPointsSlider->setMaximum(divisionMax);
  divisionPointsSlider->setStep(divisionStep);

  addButton->onPress(&addCurve, std::ref(curves), curvesListBox);
  deleteButton->onPress(&removeCurve, std::ref(curves), curvesListBox);
  addRemoveNodesButton->onPress(&switchMode, EditorMode::AddRmNodes, modeText);
  moveNodesButton->onPress(&switchMode, EditorMode::MoveNodes, modeText);
  moveCurveButton->onPress(&switchMode, EditorMode::MoveCurve, modeText);
  rotateCurveButton->onPress(&switchMode, EditorMode::RotateCurve, modeText);
  divideModeButton->onPress(&switchMode, EditorMode::DivideBezier, modeText);
  curveTypesComboBox->onItemSelect(&pickCurveType);
  curveTypesComboBox->addItem("Polyline");
  curveTypesComboBox->addItem("LagrangeInterpolation");
  curveTypesComboBox->addItem("Bezier");
  curvesListBox->onItemSelect(&selectCurve, std::ref(curves), curveOptions);
  thicknessSlider->onValueChange(&updateCurveThickness);
  

  elevateButton->onPress(&elevateBezier);
  reduceButton->onPress(&reduceBezier);
  divideButton->onPress(&divideBezier, std::ref(curves), curvesListBox, divisionPointsSlider);
  
  canvas->onMousePress(&addPoint);
  canvas->onMousePress(&selectPoint);
  canvas->onMousePress(&selectRotationPoint);
  canvas->onMouseRelease(&deselectAllPoints);
  canvas->onMouseRelease(&deselectRotationPoint);
  canvas->onRightMousePress(&removePoint);

  divideIndicator.setFillColor(sf::Color::Red);
  rotationPointSprite.setFillColor(sf::Color::Red);

  clrBackgroundBtn->onPress(&clearBackground);
  loadBackgroundBtn->onPress(&loadBackground);

 
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
    curveOptions->setVisible(activeCurve ? true : false);
    if(activeCurve)
    {
      bezierOptionsGroup->setVisible(activeCurve->getCurveType() == CurveType::Bezier);
      divideBezierGroup->setVisible(currentMode == EditorMode::DivideBezier);
    }

    canvas->clear(drawingBackgroundColor);
    if(backgroundTexture)
    {
      sf::Sprite s(*backgroundTexture);
      canvas->draw(s);
    }
    
    for(auto const& curve : curves) 
    {
      canvas->draw(*curve);
    }
    if(currentMode == EditorMode::DivideBezier)
    {
      sf::Vector2f pos = activeCurve->getBezierVal(divisionPointsSlider->getValue());
      divideIndicator.setPosition(pos.x - indicatorRadius, pos.y - indicatorRadius);
      canvas->draw(divideIndicator);
    }
    if(currentMode == EditorMode::RotateCurve)
    {
      canvas->draw(rotationPointSprite);
    }
    canvas->display();
    gui.draw();
  
    window.display();
  }
}

