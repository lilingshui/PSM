#pragma once


#include <vector>

class shape
{
public:
	virtual ~shape(){}
	virtual void draw(CDC *dc) = 0;
	virtual void get_rect(CRect &rect) = 0;
};

class shp_text : public shape
{
public:
	shp_text(CRect rect, std::string str_text, int font_size, COLORREF color = RGB(0, 0, 0), 
		unsigned int style = DT_LEFT |DT_WORDBREAK| DT_SINGLELINE|DT_VCENTER);
	virtual ~shp_text();
	virtual void draw(CDC *dc);
	virtual void get_rect(CRect &rect){rect = rect_;}
private:
	std::string str_text_;
	CRect rect_;
	COLORREF font_color_;
	int font_size_;
	unsigned int style_;
};

class shp_table : public shape
{
public:
	shp_table(std::vector<int> vec_x, std::vector<int> vec_y, int x=0, int y=0);
	virtual ~shp_table();
	virtual void draw(CDC *dc);
	virtual void get_rect(CRect &rect);
	void set_cell(int row, int col, shape *shp){cells_[row][col] = shp;}
	void set_text(int row, int col, std::string str_text, COLORREF color = RGB(0, 0, 0), 
		unsigned int style = DT_LEFT |DT_WORDBREAK| DT_SINGLELINE|DT_VCENTER);
	CPoint get_cell_point(int row, int col);
private:
	std::vector<int> vec_x_;
	std::vector<int> vec_y_;
	int pos_x_;
	int pos_y_;
	shape *cells_[20][8];
};

class shp_set : public shape
{
public:
	shp_set();
	virtual ~shp_set();
	virtual void draw(CDC *dc);
	virtual void get_rect(CRect &rect);
	void add(shape* shp){shapes_.push_back(shp);}
private:
	std::vector<shape*> shapes_;
};

//-------------------------------------------------------------------------
class shape_mgr
{
public:
	std::vector<shape*> shapes_;
	shape_mgr(void);
	~shape_mgr(void);
	void get_image(CDC *dc, CRect rect, int pos);
	void get_rect(CRect &rect);
private:
	void draw(CDC *dc);

};