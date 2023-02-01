//
// Created by Dreamtowards on 2023/2/1.
//

#ifndef ETHERTIA_MOD_H
#define ETHERTIA_MOD_H

class Mod
{
public:

    class Manifest
    {
    public:

        std::string id;
        std::string name;
        std::string version;
        std::string website;

    };

    Mod::Manifest manifest;

};

#endif //ETHERTIA_MOD_H
