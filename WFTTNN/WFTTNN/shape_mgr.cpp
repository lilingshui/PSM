#include "stdafx.h"
#include "shape_mgr.h"
#include "resource.h"

//--------------------------------------------------------------------------------
shp_text::shp_text(CRect rect, std::string str_text, int font_size, COLORREF color, unsigned int style)
	: rect_(rect)
	, str_text_(str_text)
	, font_size_(font_size)
	, font_color_(color)
	, style_(style)
{
}

shp_text::~shp_text()
{
}

void shp_text::draw(CDC *dc)
{
	CFont font;
	font.CreatePointFont(font_size_, "ËÎÌו", NULL);
	dc->SelectObject(&font);
	dc->SetBkMode(TRANSPARENT);
	dc->SetTextColor(font_color_);
	dc->DrawText(str_text_.c_str(), &rect_, style_);
}

//---------------------------------------------------------------------------------------------

shp_table::shp_table(std::vector<int> vec_x, std::vector<int> vec_y, int x, int y)
	: pos_x_(x)
	, pos_y_(y)
	, vec_x_(vec_x)
	, vec_y_(vec_y)
{
	memset(cells_, 0, 160*4);
}

shp_table::~shp_table()
{
	for (int k=0; k<20; k++)
	{
		for (int i=0; i<8; i++)
		{
			delete cells_[k][i];
		}
	}
}

void shp_table::draw(CDC *dc)
{
	for (int k=0; k<20; k++)
	{
		for (int i=0; i<8; i++)
		{
			if (cells_[k][i])
				cells_[k][i]->draw(dc);
		}
	}

	CPen pen(PS_SOLID, 2, RGB(0, 0, 0));
	dc->SelectObject(&pen);
	int y1 = pos_y_;
	int y2 = pos_y_;
	for (int i=0; i<vec_y_.size(); i++)
		y2 += vec_y_[i];
	int x = pos_x_;
	dc->MoveTo(x, y1);
	dc->LineTo(x, y2);
	for (int i=0; i<vec_x_.size(); i++)
	{
		x += vec_x_[i];
		dc->MoveTo(x, y1);
		dc->LineTo(x, y2);
	}

	int x1 = pos_x_;
	int x2 = pos_x_;
	for (int i=0; i<vec_x_.size(); i++)
		x2 += vec_x_[i];
	int y = pos_y_;
	dc->MoveTo(x1, y);
	dc->LineTo(x2, y);
	for (int i=0; i<vec_y_.size(); i++)
	{
		y += vec_y_[i];
		dc->MoveTo(x1, y);
		dc->LineTo(x2, y);
	}
}

void shp_table::get_rect(CRect &rect)
{
	rect.left = pos_x_;
	rect.top = pos_y_;
	rect.right = pos_x_;
	rect.bottom = pos_y_;
	for (int i=0; i<vec_x_.size(); i++)
		rect.right += vec_x_[i];
	for (int i=0; i<vec_y_.size(); i++)
		rect.bottom += vec_y_[i];
}

void shp_table::set_text(int row, int col, std::string str_text, COLORREF color, unsigned int style)
{
	CRect rc;
	CPoint cell_pt = get_cell_point(row, col);
	if (style & DT_CENTER)
		rc.left = cell_pt.x;
	else
		rc.left = cell_pt.x + 6;
	rc.top = cell_pt.y;
	rc.right = rc.left+vec_x_[col];
	rc.bottom  = rc.top + vec_y_[row];
	shp_text *cell_text = new shp_text(rc, str_text, 120, color, style);
	cells_[row][col] = cell_text;
}

CPoint shp_table::get_cell_point(int row, int col)
{
	CPoint pt(pos_x_, pos_y_);
	for (int i=0; i<col; i++)
		pt.x += vec_x_[i];
	for (int i=0; i<row; i++)
		pt.y += vec_y_[i];
	return pt;
}

//----------------------------------------------------------------------------------------------
shp_set::shp_set()
{

}

shp_set::~shp_set()
{
	for (int i=0; i<shapes_.size(); i++)
	{
		delete shapes_[i];
	}
}

void shp_set::draw(CDC *dc)
{
	for (int i=0; i<shapes_.size(); i++)
	{
		shapes_[i]->draw(dc);
	}
}

void shp_set::get_rect(CRect &rect)
{
	rect.SetRect(0, 0, 0 ,0);
	if (shapes_.size() == 0)
		return;
	shapes_[0]->get_rect(rect);
	for (int i=1; i<shapes_.size(); i++)
	{
		CRect rc;
		shapes_[i]->get_rect(rc);
		rect.UnionRect(&rect, &rc);
	}
}

//-----------------------------------------------------------------------------------------------
shape_mgr::shape_mgr(void)
{

}


shape_mgr::~shape_mgr(void)
{
	for (int i=0; i<shapes_.size(); i++)
	{
		delete shapes_[i];
	}
}

void shape_mgr::draw(CDC *dc)
{
	for (int i=0; i<shapes_.size(); i++)
	{
		shapes_[i]->draw(dc);
	}
}

void shape_mgr::get_rect(CRect &rect)
{
	rect.SetRect(0, 0, 0 ,0);
	if (shapes_.size() == 0)
		return;
	shapes_[0]->get_rect(rect);
	for (int i=1; i<shapes_.size(); i++)
	{
		CRect rc;
		shapes_[i]->get_rect(rc);
		rect.UnionRect(&rect, &rc);
	}
}

void shape_mgr::get_image(CDC *dc, CRect rect, int pos)
{
	CDC memDC;
	memDC.CreateCompatibleDC(dc);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(dc,rect.Width(),rect.Height()+3000);
	memDC.SelectObject(&bmp);
	CBrush brush(RGB(255,255,255));
	memDC.FillRect(CRect(0,0,rect.Width(),rect.Height()+3000),&brush);

	draw(&memDC);
	dc->BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, pos, SRCCOPY);

	memDC.DeleteDC();
	bmp.DeleteObject();
}
