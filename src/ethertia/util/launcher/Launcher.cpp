

#include <ethertia/util/Log.h>

#include <libui/ui.h>
#include <tinyfd/tinyfiledialogs.h>

// What


static int onClosing(uiWindow* w, void *data) {
    uiQuit();
    return 1;
}

static int onShouldQuit(void *data)
{
    uiWindow* mainwin = uiWindow(data);

    uiControlDestroy(uiControl(mainwin));
    return 1;
}

static uiControl *makeBasicControlsPage(void)
{
    uiBox *vbox;
    uiBox *hbox;
    uiGroup *group;
    uiForm *entryForm;

    vbox = uiNewVerticalBox();
    uiBoxSetPadded(vbox, 1);

    hbox = uiNewHorizontalBox();
    uiBoxSetPadded(hbox, 1);
    uiBoxAppend(vbox, uiControl(hbox), 0);

    uiBoxAppend(hbox,
                uiControl(uiNewButton("Button")),
                0);
    uiBoxAppend(hbox,
                uiControl(uiNewCheckbox("Checkbox")),
                0);

    uiBoxAppend(vbox,
                uiControl(uiNewLabel("This is a label. Right now, labels can only span one line.")),
                0);

    uiBoxAppend(vbox,
                uiControl(uiNewHorizontalSeparator()),
                0);

    group = uiNewGroup("Entries");
    uiGroupSetMargined(group, 1);
    uiBoxAppend(vbox, uiControl(group), 1);

    entryForm = uiNewForm();
    uiFormSetPadded(entryForm, 1);
    uiGroupSetChild(group, uiControl(entryForm));

    uiFormAppend(entryForm,
                 "Entry",
                 uiControl(uiNewEntry()),
                 0);
    uiFormAppend(entryForm,
                 "Password Entry",
                 uiControl(uiNewPasswordEntry()),
                 0);
    uiFormAppend(entryForm,
                 "Search Entry",
                 uiControl(uiNewSearchEntry()),
                 0);
    uiFormAppend(entryForm,
                 "Multiline Entry",
                 uiControl(uiNewMultilineEntry()),
                 1);
    uiFormAppend(entryForm,
                 "Multiline Entry No Wrap",
                 uiControl(uiNewNonWrappingMultilineEntry()),
                 1);

    return uiControl(vbox);
}

class Launcher
{
public:

    uiWindow* mainwin = nullptr;

    Launcher()
    {
        mainwin = uiNewWindow("Ethertia Launcher 0.0.2", 820, 480, true);
        uiWindowOnClosing(mainwin, onClosing, nullptr);
        uiOnShouldQuit(onShouldQuit, mainwin);
        uiWindowSetMargined(mainwin, true);

        {
            uiBox* vbox = uiNewVerticalBox();
            uiWindowSetChild(mainwin, uiControl(vbox));
            uiBoxSetPadded(vbox, true);

            {
                uiTab* tab = uiNewTab();
                uiBoxAppend(vbox, uiControl(tab), 0);

                uiTabAppend(tab, "Update News", makeBasicControlsPage());
                uiTabSetMargined(tab, 0, true);

                uiTabAppend(tab, "Development Console", makeBasicControlsPage());
                uiTabSetMargined(tab, 1, true);

                uiTabAppend(tab, "Profile Editor", uiControl(uiNewButton("Sth")));
                uiTabSetMargined(tab, 2, true);

                uiTabAppend(tab, "Local Versions Management", uiControl(uiNewButton("Sth")));
                uiTabSetMargined(tab, 3, true);
            }

            {
                uiBox* hbox = uiNewHorizontalBox();
                uiBoxAppend(vbox, uiControl(hbox), 0);

                uiBoxAppend(hbox,
                            uiControl(uiNewButton("Button")),
                            0);
                uiBoxAppend(hbox,
                            uiControl(uiNewCheckbox("Checkbox")),
                            0);
            }

        }


        uiControlShow(uiControl(mainwin));
        uiMain();
    }
};

int main() {

    uiInitOptions options{};

    const char* err;
    if ((err= uiInit(&options))) {
        Log::info("Failed Init libui. {}", err);
        uiFreeInitError(err);
        return 1;
    }

    Launcher lcr{};

    return 0;
}