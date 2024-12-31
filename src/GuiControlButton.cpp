#include "GuiControlButton.h"
#include "Render.h"
#include "Engine.h"
#include "Audio.h"
#include "Scene.h"
#include "GuiManager.h"

GuiControlButton::GuiControlButton(int id, SDL_Rect bounds, const char* text) : GuiControl(GuiControlType::BUTTON, id)
{
	SelectUp = Engine::GetInstance().audio.get()->LoadFx("Assets/Audio/Fx/SelectUp.wav");
	this->bounds = bounds;
	this->text = text;

	canClick = true;
	drawBasic = false;
	soundPlayed = false;
}

GuiControlButton::~GuiControlButton()
{

}
bool GuiControlButton::Update(float dt)
{
	if (state == GuiControlState::DISABLED)
	{
		return false; // Sal de la funci�n y evita actualizar l�gica innecesaria
	}

	if (state != GuiControlState::DISABLED)
	{
		Vector2D mousePos = Engine::GetInstance().input->GetMousePosition();

		// Verifica si el mouse est� dentro de los l�mites del bot�n
		bool mouseOverButton = mousePos.getX() > bounds.x && mousePos.getX() < bounds.x + bounds.w &&
			mousePos.getY() > bounds.y && mousePos.getY() < bounds.y + bounds.h;

		if (mouseOverButton)
		{
			if (state != GuiControlState::FOCUSED)  // El estado solo cambia la primera vez que el rat�n entra
			{
				state = GuiControlState::FOCUSED;				
			}

			if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)
			{
				state = GuiControlState::PRESSED;
				// Reproducir el sonido solo una vez al entrar al �rea del bot�n
				if (!soundPlayed) // Solo reproducir si no se ha reproducido ya
				{
					Engine::GetInstance().audio->PlayFx(SelectUp);
					soundPlayed = true; // Marcar que el sonido se ha reproducido
				}
			}

			if (Engine::GetInstance().input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)
			{
				NotifyObserver();
			}
		}
		else
		{
			// Si el rat�n sale del �rea del bot�n, restablece el estado y permite la reproducci�n del sonido en la siguiente vez que entre
			if (state != GuiControlState::NORMAL)
			{
				state = GuiControlState::NORMAL;
				soundPlayed = false; // Restablecer la bandera cuando el rat�n sale del �rea
			}
		}
	}

	return false;
}

void GuiControlButton::Draw()
{
	//L16: TODO 4: Draw the button according the GuiControl State
	if (state == GuiControlState::DISABLED)
	{
		return; // No hacer nada si el bot�n est� deshabilitado
	}

	if (id == 6 || id == 7) {
		// Hacer que este bot�n no tenga transparencia (alfa = 255)
		switch (state)
		{
		case GuiControlState::DISABLED:
			Engine::GetInstance().render->DrawRectangle(bounds, 255, 0, 0, 50, true, true); // Rojo para botones deshabilitados (depuraci�n)

			break;
		case GuiControlState::NORMAL:
			Engine::GetInstance().render->DrawRectangle(bounds, 195, 159, 129, 100, true, false); // Alfa = 255 (sin transparencia)
			break;
		case GuiControlState::FOCUSED:
			Engine::GetInstance().render->DrawRectangle(bounds, 141, 73, 37, 100, true, false); // Alfa = 255 (sin transparencia)
			break;
		case GuiControlState::PRESSED:
			Engine::GetInstance().render->DrawRectangle(bounds, 200, 200, 200, 100, true, false); // Alfa = 255 (sin transparencia)
			break;
		}
	}

	else if (id == 8 || id == 9) {
		// Hacer que este bot�n no tenga transparencia (alfa = 255)
		switch (state)
		{
		case GuiControlState::DISABLED:
			Engine::GetInstance().render->DrawRectangle(bounds, 255, 0, 0, 50, true, true); // Rojo para botones deshabilitados (depuraci�n)

			break;

		case GuiControlState::NORMAL:
			Engine::GetInstance().render->DrawRectangle(bounds, 242, 240, 235, 255, true, false); // Alfa = 255 (sin transparencia)
			break;
		case GuiControlState::FOCUSED:
			Engine::GetInstance().render->DrawRectangle(bounds, 255, 252, 240, 255, true, false); // Alfa = 255 (sin transparencia)
			break;
		case GuiControlState::PRESSED:
			Engine::GetInstance().render->DrawRectangle(bounds, 250, 251, 253, 255, true, false); // Alfa = 255 (sin transparencia)
			break;
		}
	}
	else {
		switch (state)
		{
		case GuiControlState::DISABLED:
			Engine::GetInstance().render->DrawRectangle(bounds, 255, 0, 0, 50, true, true); // Rojo para botones deshabilitados (depuraci�n)

			break;
		case GuiControlState::NORMAL:
			Engine::GetInstance().render->DrawRectangle(bounds, 200, 200, 200, 100, true, false);
			break;
		case GuiControlState::FOCUSED:
		
			Engine::GetInstance().render->DrawRectangle(bounds, 0, 180, 255, 100, true, false);
			break;
		case GuiControlState::PRESSED:
			Engine::GetInstance().render->DrawRectangle(bounds, 0, 255, 150, 100, true, false);
			break;
		}
	}

	// Dibujar el texto del bot�n
	Engine::GetInstance().render->DrawText(text.c_str(), bounds.x, bounds.y, bounds.w, bounds.h);
}
