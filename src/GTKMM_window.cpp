#include "GTKMM_window.h"

GTKMMWindow::GTKMMWindow()
{
	path = "";
	//refresh().connect(sigc::mem_fun((*this), &GTKMMWindow::reload));

	/* Set some window properties */
	set_title("result");
	set_size_request(300, 300);

	/* Sets the border width of the window. */
	set_border_width(10);

	add(m_Frame);

	/* Set the frames label */
	m_Frame.set_label("Line chart");

	/* Align the label at the right of the frame */
	//m_Frame.set_label_align(Gtk::ALIGN_END, Gtk::ALIGN_START);

	/* Set the style of the frame */
	m_Frame.set_shadow_type(Gtk::SHADOW_ETCHED_OUT);

	//m_Frame.show_all_children();

	show_all_children();
}

GTKMMWindow::~GTKMMWindow()
{
	
}

GTKMMWindow::Signals GTKMMWindow::refresh()
{
	return refresh_signal;
}

void GTKMMWindow::reload()
{
	m_Image = Gtk::Image(path + "/result.png");
	m_Frame.add(m_Image);
	show_all_children();
}

void GTKMMWindow::set_path(string path)
{
	this->path = path;
}
