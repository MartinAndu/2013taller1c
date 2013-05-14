/*
 * ChatWindowsView.cpp
 *
 *  Created on: 01/05/2013
 *      Author: damian
 */

#include <view/networking/ChatWindowsView.h>
#include <view/TextHandler.h>
#define FONT "resources/fonts/robot.ttf"
#define WINDOWSHEIGHT 100

ChatWindowsView::~ChatWindowsView() {
	// TODO Auto-generated destructor stub
}

ChatWindowsView::ChatWindowsView(){

	this->state = true;
	this->chat=NULL;

	this->clip.x = 0;
	this->clip.y = 0;
	this->clip.h = WINDOWSHEIGHT;

	font = TTF_OpenFont(FONT,20);
	SDL_Colour colour = {1, 1, 1 };
	//cout << "asigna color" << endl;
	this->text_colour = colour;
}
bool ChatWindowsView::isActive()
{
	return this->state;
}
void ChatWindowsView::drawChatView(SDL_Surface* screen)
{
	if(this->isActive())
	{
		this->drawChatWindow(screen);
		this->text=chat->getMessage();
		this->draw_text(this->text);
		if(this->chat->NewLine())
		{

		}
	}
}
void ChatWindowsView::setChat(Chat* chat)
{
	this->chat=chat;
}
bool ChatWindowsView::drawChatWindow(SDL_Surface* screen) {
	this->clip.w = screen->w;
	this->_screen=screen;
	return SDL_FillRect(screen, &this->clip, 0x0D7A3E);
}

bool ChatWindowsView::draw_text(string texto) {
	string display;
	display = texto;
	SDL_Surface* msg ;
	msg = TTF_RenderText_Solid(this->font, texto.c_str(), this->text_colour);
	SDL_Rect tmp = this->clip;
	tmp.x += 5;
	tmp.y +=80;
	return SDL_BlitSurface(msg, NULL, this->_screen, &tmp);
}

std::string ChatWindowsView::get_text(void) {
	return this->text;
}

