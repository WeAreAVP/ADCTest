#ifndef RIMPANEL_H
#define RIMPANEL_H

#include <wx/window.h>
#include <wx/panel.h>
#include <wx/dcbuffer.h>


class RimPanel : public wxPanel
{
    public:
        RimPanel(wxWindow *parent,
                 wxWindowID id=wxID_ANY,
                 const wxPoint &pos=wxDefaultPosition,
                 const wxSize &size=wxDefaultSize,
                 long style=wxTAB_TRAVERSAL,
                 const wxString &name=wxPanelNameStr);

        virtual ~RimPanel();

        void setBGColours( unsigned char redT, unsigned char greenT, unsigned char blueT, unsigned char redB, unsigned char greenB, unsigned char blueB );

        void paintEvent(wxPaintEvent & evt);
        void OnSize(wxSizeEvent &event);

    DECLARE_EVENT_TABLE();

    protected:
        void render(wxDC& dc);

        wxColour mBGColourTop;
        wxColour mBGColourBottom;
        wxColour mFGColour;

    private:
};

#endif // RIMPANEL_H
