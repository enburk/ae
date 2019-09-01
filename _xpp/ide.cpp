#include "gui_window.h"
//#include "ide_test.h"
//#include "ide_edit.h"
//#include "ide_flist.h"
//using namespace ide;

struct IDE : Widget
{
    IDE
    {
        Window = this;
    }
};

IDE ide;

//Editor editor;
//Flist flist;
//
//void sys::mouse::on::clickclick(XY p, char button)
//{
//    if (XYXY(editor.frame).include(p)) editor.on_mouse_clickclick(p, button); else
//    if (XYXY(flist .frame).include(p)) flist .on_mouse_clickclick(p, button); else
//}
//void sys::mouse::on::press(XY p, char button, bool down) {
//
//    capture
//}
//void sys::mouse::on::move(XY p) {
//    test::mouse::on::move(p);
//}
//
//void sys::window::on::resize() {
//    test::window::on::resize();
//}
//void sys::window::on::redraw(XYXY r) {
//    test::window::on::redraw(r);
//}

// void sys::mouse::on::clickclick(XY p, char button) {
// if (test::mouse::on::clickclick(p, button)) return;
// }
// void sys::mouse::on::press(XY p, char button, bool down) {
//     test::mouse::on::press(p, button, down);
// }
// void sys::mouse::on::move(XY p) {
//     test::mouse::on::move(p);
// }
// 
// void sys::window::on::resize() {
//     test::window::on::resize();
// }
// void sys::window::on::redraw(XYXY r) {
//     test::window::on::redraw(r);
// }


#include "doc_lexica_txt.h"
