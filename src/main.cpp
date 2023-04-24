#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include "curve.hpp"
#include <string>

enum EditionMode { None, Add, Move};

static const sf::Color drawingBackgroundColor = sf::Color(200,200,200);
Curve* activeCurve = nullptr;
static EditionMode currentMode = EditionMode::None;

static int curvesNum = 0;

void addPoint(tgui::Vector2f pos)
{
    if(currentMode != EditionMode::Add) return;
    Node node(pos);
    if(activeCurve)
    {
        activeCurve->addNode(node);
    }
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
    curve.isSelected = false;
} 

void removeCurve(std::vector<Curve>& curves, tgui::ListBox::Ptr listBox)
{
  activeCurve = nullptr;
  int index = listBox->getSelectedItemIndex();
  std::cout<<index<<std::endl;
  curves.erase(curves.begin() + index);
  listBox->removeItemByIndex(index);
  listBox->setSelectedItemByIndex(-1);
  currentMode = EditionMode::None;
}


void selectCurve(std::vector<Curve>& curves, int index)
{
  if(curves.empty()) return;
  if(index > curves.size()) return; 
  for(auto& curve : curves)
    curve.isSelected = false;
  activeCurve = &curves[index];
  activeCurve->isSelected = true;
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

    addButton->onPress(&addCurve, std::ref(curves), curvesListBox);
    deleteButton->onPress(&removeCurve, std::ref(curves), curvesListBox);
    modeSwitchButton->onPress(&switchMode);
    curvesListBox->onItemSelect(&selectCurve, std::ref(curves));
    canvas->onMousePress(&addPoint);
    
    // Main loop
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            gui.handleEvent(event);
    
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Set conditionally visible elements
        deleteButton->setEnabled(curvesListBox->getSelectedItemIndex() != -1);
        inspectorGroup->setVisible(currentMode != EditionMode::None);
        if(currentMode == EditionMode::Add)
          modeSwitchButton->setText("Adding points");
        if(currentMode == EditionMode::Move)
          modeSwitchButton->setText("Moving points");
          
        window.clear();
        canvas->clear(drawingBackgroundColor);
        for(auto const& curve : curves) 
            canvas->draw(curve);
        canvas->display();
        gui.draw();
    
        window.display();
    }
}

