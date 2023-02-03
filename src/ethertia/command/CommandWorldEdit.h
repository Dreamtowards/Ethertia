//
// Created by Dreamtowards on 2023/2/3.
//

#ifndef ETHERTIA_COMMANDWORLDEDIT_H
#define ETHERTIA_COMMANDWORLDEDIT_H

#include <ethertia/command/Command.h>

#include <ethertia/material/Material.h>
#include <ethertia/util/AABB.h>

class WorldEdit {
public:

    inline static AABB selection;

};

class CommandWEPos1 : public Command {
public:
    static glm::vec3 pos(Args args) {
        glm::vec3 p = Ethertia::getCamera()->position;
        if (args.size() == 2 && args[1] == "-l") {
            p = Ethertia::getCursor().position;
        }
        return p;
    }

    //  //pos1 [l]   # l: use lookat pos instead of camera pos.
    void onCommand(Args args) override {
        WorldEdit::selection.set(pos(args));
        SendMessage("Set pos1.");
    }
};

class CommandWEPos2 : public Command {
public:
    //  //pos2 [l]
    void onCommand(Args args) override {
        WorldEdit::selection.wrap(WorldEdit::selection.min, CommandWEPos1::pos(args));
        SendMessage("Set pos2");
    }
};

class CommandWESet : public Command {
public:
    void onCommand(Args args) override
    {
        std::string id = args[1];
        const Material* mtl = Material::REGISTRY.get(id);
        if (!mtl) {
            SendMessage("Material '{}' not found.", id);
            return;
        }

        AABB& sel = WorldEdit::selection;
        if (sel.empty()) {
            Ethertia::notifyMessage("No selection.");
            return;
        }

        bool fillBlock = false;
        if (Collections::find(args, std::string("-b")) != -1) {
            fillBlock = true;
        }

        AABB::forEach(glm::floor(sel.min), glm::ceil(sel.max), [=](glm::vec3 p) {

            Cell& c = Ethertia::getWorld()->getCell(p);

            c.mtl = mtl;
            if (fillBlock) {
                c.exp_meta = 1;
                c.density = 0;
            } else {
                c.exp_meta = 0;
                c.density = 0.5;
            }

            Ethertia::getWorld()->requestRemodel(p, false);

        }, 1.0f);

        Ethertia::notifyMessage("Filled.");
        WorldEdit::selection.set({});  // clear selection
    }
};

#endif //ETHERTIA_COMMANDWORLDEDIT_H
