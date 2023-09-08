//
// Created by Dreamtowards on 2023/4/5.
//

#ifndef ETHERTIA_ADVANCEMENT_H
#define ETHERTIA_ADVANCEMENT_H

#include <vector>
#include <string>

class Advancement
{
public:

    Advancement* m_Parent;
    std::vector<Advancement> m_Children;  // ?ptr elements

    std::string m_Name;
    std::string m_Description;

    // m_Rewards;



};

#endif //ETHERTIA_ADVANCEMENT_H
