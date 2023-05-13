#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <string>

#include "constants.hpp"
#include "curve.hpp"
#include "point.hpp"

//TODO
//Create a class for Editor with all crucial elements
enum EditionMode { None, Add, Move};

Curve* activeCurve = nullptr;
Node* activePoint = nullptr;

sf::Vector2f initialMousePos;
static EditionMode currentMode = EditionMode::None;
static int curvesNum = 0;

inline sf::Vector2f sfmlMousePosToTgui(sf::Vector2f originalPos, float windowWidth)
{
  return originalPos - sf::Vector2f(windowWidth * 0.2, 0);
}

// Callbacks
void addPoint(tgui::Vector2f pos)
{
    if(currentMode != EditionMode::Add) return;
    Node node(pos);
    if(activeCurve)
    {
        activeCurve->addNode(node);
    }
}

void removePoint(tgui::Vector2f pos)
{
  if(currentMode != EditionMode::Move) return;
  if(activePoint != nullptr) return;
  activeCurve->removeClickedNode(pos); 
}

void selectPoint(tgui::Vector2f pos)
{
  if(currentMode != EditionMode::Move) return;
  if(activePoint != nullptr) return;
  initialMousePos = pos;
  activePoint = activeCurve->findClickedNode(pos);
  if(activePoint)
  {
    activePoint->select();
  }
}

void deselectAllPoints()
{
  if(currentMode != EditionMode::Move) return;
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
  currentMode = EditionMode::None;
  Curve c;
  curves.push_back(c);
  for(auto& curve : curves)
    curve.deselect();
} 

void removeCurve(std::vector<Curve>& curves, tgui::ListBox::Ptr listBox)
{
  activeCurve = nullptr;
  int index = listBox->getSelectedItemIndex();
  curves.erase(curves.begin() + index);
  listBox->removeItemByIndex(index);
  listBox->setSelectedItemByIndex(-1);
  currentMode = EditionMode::None;
}

void selectCurve(std::vector<Curve>& curves, int index)
{
  if(curves.empty()) return;
  if(index > curves.size()) return; 
  if(activeCurve)
  {
    activeCurve->deselect();
  }
  activeCurve = &curves[index];
  activeCurve->select();
  currentMode = EditionMode::Add;
}

void switchMode()
{
  if(currentMode == EditionMode::Add)
  {
    currentMode = EditionMode::Move; 
  }
  else if(currentMode == EditionMode::Move)
  {
    currentMode = EditionMode::Add;
  }
}

void pickCurveType(int index)
{
  switch(index)
  {
    case 0:
    activeCurve->changeCurveType(CurveType::Polyline);
    break;
    case 1:
    activeCurve->changeCurveType(CurveType::LagrangeInterpolation);
    break;
  }
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
    auto addButton = gui.get<tgui::Button>("AddButton");
    auto deleteButton = gui.get<tgui::Button>("DeleteButton");
    auto inspectorGroup = gui.get<tgui::Group>("InspectorGroup");
    auto modeSwitchButton = gui.get<tgui::Button>("EditionModeButton");
    auto curveTypeComboBox= gui.get<tgui::ComboBox>("CurveTypeCombo");

    addButton->onPress(&addCurve, std::ref(curves), curvesListBox);
    deleteButton->onPress(&removeCurve, std::ref(curves), curvesListBox);
    modeSwitchButton->onPress(&switchMode);
    curvesListBox->onItemSelect(&selectCurve, std::ref(curves));
    curveTypeComboBox->onItemSelect(&pickCurveType);
    curveTypeComboBox->addItem("Polyline");
    curveTypeComboBox->addItem("Interpolated");
    
    canvas->onMousePress(&addPoint);
    canvas->onMousePress(&selectPoint);
    canvas->onMouseRelease(&deselectAllPoints);
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
            if ((event.type == sf::Event::MouseMoved) && 
                (activePoint != nullptr) && 
                (activeCurve != nullptr) && 
                (currentMode == EditionMode::Move))
            {
              activePoint->setPosition(event.mouseMove.x - 0.2 * window.getSize().x, event.mouseMove.y );
              activeCurve->updateCurve();
            }
        }

        // Set conditionally visible elements
        deleteButton->setEnabled(curvesListBox->getSelectedItemIndex() != -1);
        inspectorGroup->setVisible(currentMode != EditionMode::None);
        if(currentMode == EditionMode::Add)
          modeSwitchButton->setText("Adding points");
        if(currentMode == EditionMode::Move)
          modeSwitchButton->setText("Editing points");
          
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

