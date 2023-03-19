#include "account_widget.h"

#include <pangomm/attributes.h>
#include <pangomm/attrlist.h>

AccountWidget::AccountWidget()
{
    Pango::AttrList attribs;
    auto sizeAttr = Pango::Attribute::create_attr_size_absolute(20 * PANGO_SCALE);
    attribs.insert(sizeAttr);
    set_margin(10);
    set_orientation(Gtk::Orientation::VERTICAL);
    set_homogeneous(true);
    name.set_halign(Gtk::Align::START);
    name.set_overflow(Gtk::Overflow::HIDDEN);
    append(name);
    code.set_attributes(attribs);
    code.set_halign(Gtk::Align::END);
    append(code);
};

AccountWidget::~AccountWidget() {};

void AccountWidget::setName(const Glib::ustring& newName)
{
    name.set_text(newName);
}

void AccountWidget::setCode(const Glib::ustring& newCode)
{
    code.set_text(newCode);
}
