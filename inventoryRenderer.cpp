#include "precomp.h"
#include "inventoryRenderer.h"
#include "TextRenderer.h"
#include "renderingConst.h"
#include "Player.h"
#include "commonTypes.h"
#include "basePickupable.h"

InventoryRenderer::InventoryRenderer(const Player* p_ptrplayer, TextRenderer* p_textRenderer, Surface* p_ptrScreen)
{
	m_ptrplayer = p_ptrplayer;
	m_textRenderer = p_textRenderer;
	m_ptrScreen = p_ptrScreen;
	m_inventoryHeader = "lorem";



}

void InventoryRenderer::SetData(bool m_renderWeapons)
{
	if (m_renderWeapons)
	{
		m_inventoryHeader = "weapon select";
	}
	else
	{
		m_inventoryHeader = "item select";
	}

	if (renderedItems != nullptr && renderedItems->data != nullptr)
	{
		delete[] renderedItems->data;
	}
	renderedItems = m_ptrplayer->GetRenderedItems(m_renderWeapons);
}

InventoryRenderer::~InventoryRenderer()
{
	if (renderedItems != nullptr && renderedItems->data != nullptr)
	{
		delete[] renderedItems->data;
	}
	//delete renderedItems;
}

void InventoryRenderer::Render()
{
	m_textRenderer->RenderText({ 150, 100 }, { renderingConst::TEXT_SIZE, renderingConst::TEXT_SIZE }, m_inventoryHeader);
	m_ptrScreen->Bar(MAP_RENDERING_START, ROOM_SIZE_IN_PIXELS - 200, ROOM_SIZE_IN_PIXELS, ROOM_SIZE_IN_PIXELS, DARK_BLUE);
	for (int i = 0; i < 3; i++)
	{
		m_ptrScreen->Box(MAP_RENDERING_START + i, ROOM_SIZE_IN_PIXELS + i - 200, ROOM_SIZE_IN_PIXELS + i, ROOM_SIZE_IN_PIXELS + i, 0xffffff);
	}
	int2 selectionDot = m_ptrplayer->GetInventoryIndex();
	selectionDot.x *= renderingConst::SELECTION_MULTI_X;
	selectionDot.y *= renderingConst::SELECTION_MULTI_y;

	selectionDot.x += renderingConst::SELECTION_DOT_START_X;
	selectionDot.y += renderingConst::SELECTION_START_Y;

	m_ptrScreen->Bar(selectionDot.x, selectionDot.y, selectionDot.x + 20, selectionDot.y + 20, 0xffffff);

	for (int i = 0; i < renderedItems->size; i++)
	{
		if (renderedItems->data[i] == nullptr)
		{
			continue;
		}
		int2 pos = { i / renderingConst::INVENTORY_PITCH, i % renderingConst::INVENTORY_PITCH };
		pos.x *= renderingConst::SELECTION_MULTI_X;
		pos.y *= renderingConst::SELECTION_MULTI_y;

		pos.x += renderingConst::SELECTION_ICON_START_X;
		pos.y += renderingConst::SELECTION_START_Y;

		renderedItems->data[i]->RenderIcon(pos, { 150, 80 });

	}


}
