#ifndef GTKMM_WINDOW_H
#define GTKMM_WINDOW_H

#include <gtkmm.h>
#include <string>
//#include <gtkmm/window.h>
//#include <gtkmm/frame.h>

using namespace std;

class GTKMMWindow : public Gtk::Window
{
	private:
		string path;

	public:
		GTKMMWindow();
		virtual ~GTKMMWindow();

		typedef sigc::signal<void, bool, int> Signals;
		Signals refresh();

		void reload();
		void set_path(string path);

	protected:
		Signals refresh_signal;
		//Child widgets:
		Gtk::Frame m_Frame;
		Gtk::Image m_Image;
};

#endif //GTKMM_WINDOW_H
