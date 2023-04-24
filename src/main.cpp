#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include "curve.hpp"
#include <string>

Curve* activeCurve = nullptr;

static int curvesNum = 0;

void addPoint(tgui::Vector2f pos)
{
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
}


void selectCurve(std::vector<Curve>& curves, int index)
{
  if(curves.empty()) return;
  if(index > curves.size()) return; 
  for(auto& curve : curves)
    curve.isSelected = false;
  activeCurve = &curves[index];
  activeCurve->isSelected = true;
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
    canvas->onMousePress(&addPoint);
    gui.add(canvas);

    auto curvesListBox = gui.get<tgui::ListBox>("CurvesListBox");
    auto addButton = gui.get<tgui::Button>("AddButton");
    auto deleteButton = gui.get<tgui::Button>("DeleteButton");
    addButton->onPress(&addCurve, std::ref(curves), curvesListBox);
    deleteButton->onPress(&removeCurve, std::ref(curves), curvesListBox);

    curvesListBox->onItemSelect(&selectCurve, std::ref(curves));
    

    
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
        if(curvesListBox->getSelectedItemIndex() == -1)
          deleteButton->setVisible(false);
        else
          deleteButton->setVisible(true);
           
        window.clear();
        canvas->clear(sf::Color::Blue);
        for(auto const& curve : curves) 
            canvas->draw(curve);
        canvas->display();
        gui.draw();
    
        window.display();
    }
}

