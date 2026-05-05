#pragma once

#include "GridPanel.h"

namespace TeamBPathfinder
{
    using namespace System;
    using namespace System::Windows::Forms;
    using namespace System::Drawing;
    using namespace System::Drawing::Drawing2D;

    /// <summary>
    /// Draws and manages hit detection for the Game Boy-style visual chrome.
    /// </summary>
    public ref class GameBoyChrome
    {
    public:
        /// <summary>
        /// Represents the clickable chrome control region hit by the mouse.
        /// </summary>
        enum class HitResult
        {
            None,
            DPadUp,
            DPadDown,
            DPadLeft,
            DPadRight,
            ButtonA,
            ButtonB
        };

        /// <summary>
        /// Initializes a new Game Boy chrome renderer.
        /// </summary>
        /// <param name="formWidth">The width of the form.</param>
        /// <param name="formHeight">The height of the form.</param>
        /// <param name="gridTop">The top y-coordinate of the grid.</param>
        GameBoyChrome(int formWidth, int formHeight, int gridTop)
        {
            this->formWidth = formWidth;
            this->formHeight = formHeight;
            this->gridTop = gridTop;
            computeControlBounds();
        }

        /// <summary>
        /// Paints the full Game Boy-style chrome.
        /// </summary>
        /// <param name="g">The graphics surface to paint on.</param>
        void paint(Graphics^ g)
        {
            g->SmoothingMode = SmoothingMode::AntiAlias;

            paintConsoleCase(g);
            paintScreenBezel(g);
            paintBranding(g);
            paintSpeakerGrille(g);
            paintDPadAndButtons(g);
            paintStartSelectPills(g);
        }

        /// <summary>
        /// Determines which chrome control was hit by a mouse location.
        /// </summary>
        /// <param name="location">The mouse location to test.</param>
        /// <returns>The hit result for the location.</returns>
        HitResult hitTest(Point location)
        {
            if (dpadUp.Contains(location)) return HitResult::DPadUp;
            if (dpadDown.Contains(location)) return HitResult::DPadDown;
            if (dpadLeft.Contains(location)) return HitResult::DPadLeft;
            if (dpadRight.Contains(location)) return HitResult::DPadRight;

            if (isInsideCircle(location, aButtonCenter, abButtonRadius))
                return HitResult::ButtonA;
            if (isInsideCircle(location, bButtonCenter, abButtonRadius))
                return HitResult::ButtonB;

            return HitResult::None;
        }

        static const int GRID_MARGIN = 28;
        static const int MENU_HEIGHT = 24;
        static const int BRAND_TOP = MENU_HEIGHT + 14;
        static const int BRAND_HEIGHT = 48;
        static const int LABEL_ROW_Y = MENU_HEIGHT + BRAND_HEIGHT + 18;
        static const int LABEL_ROW_H = 22;
        static const int CONTROLS_GAP = 22;
        static const int CONTROLS_ROW_H = 230;
        static const int BOTTOM_BTN_H = 38;
        static const int BOTTOM_BTN_GAP = 14;

        static initonly Color PLASTIC = Color::FromArgb(196, 190, 183);
        static initonly Color PLASTIC_HI = Color::FromArgb(220, 215, 208);
        static initonly Color PLASTIC_LO = Color::FromArgb(150, 144, 138);
        static initonly Color PLASTIC_SHADE = Color::FromArgb(170, 164, 158);
        static initonly Color INK = Color::FromArgb(40, 40, 50);
        static initonly Color BUTTON_BORDER = Color::FromArgb(80, 80, 90);

    private:
        int formWidth;
        int formHeight;
        int gridTop;

        Drawing::Rectangle dpadUp;
        Drawing::Rectangle dpadDown;
        Drawing::Rectangle dpadLeft;
        Drawing::Rectangle dpadRight;
        Point aButtonCenter;
        Point bButtonCenter;
        int abButtonRadius;

        static initonly Color BEZEL = Color::FromArgb(48, 56, 80);
        static initonly Color BEZEL_DK = Color::FromArgb(28, 34, 52);
        static initonly Color BUTTON_BODY = Color::FromArgb(146, 38, 86);
        static initonly Color POWER_LED = Color::FromArgb(220, 40, 40);
        static initonly Color DPAD_DARK = Color::FromArgb(34, 34, 38);
        static initonly Color DPAD_CAP = Color::FromArgb(70, 70, 80);
        static initonly Color DPAD_TRI = Color::FromArgb(150, 150, 160);

        static const int DPAD_ARM_LENGTH = 42;
        static const int DPAD_ARM_THICKNESS = 44;
        static const int AB_BUTTON_RADIUS = 30;

        /// <summary>
        /// Determines whether a point is inside a circle.
        /// </summary>
        /// <param name="location">The point to test.</param>
        /// <param name="center">The circle center.</param>
        /// <param name="radius">The circle radius.</param>
        /// <returns>True if the point is inside the circle; otherwise, false.</returns>
        bool isInsideCircle(Point location, Point center, int radius)
        {
            int dx = location.X - center.X;
            int dy = location.Y - center.Y;
            return (dx * dx + dy * dy) <= (radius * radius);
        }

        /// <summary>
        /// Computes clickable bounds for the D-pad and action buttons.
        /// </summary>
        void computeControlBounds()
        {
            int boardPx = GridPanel::BOARD_SIZE * GridPanel::CELL_SIZE + 2;
            int controlsRowY = gridTop + boardPx + CONTROLS_GAP;

            int dpadCenterX = GRID_MARGIN + 16 + DPAD_ARM_LENGTH + 10;
            int dpadCenterY = controlsRowY + (CONTROLS_ROW_H - BOTTOM_BTN_H - 28) / 2;

            int hArmX = dpadCenterX - DPAD_ARM_LENGTH - 12;
            int hArmY = dpadCenterY - DPAD_ARM_THICKNESS / 2;

            int vArmX = dpadCenterX - DPAD_ARM_THICKNESS / 2;
            int vArmY = dpadCenterY - DPAD_ARM_LENGTH - 12;

            dpadUp = Drawing::Rectangle(vArmX, vArmY, DPAD_ARM_THICKNESS, DPAD_ARM_THICKNESS + 12);
            dpadDown = Drawing::Rectangle(vArmX, dpadCenterY, DPAD_ARM_THICKNESS, DPAD_ARM_THICKNESS + 12);
            dpadLeft = Drawing::Rectangle(hArmX, hArmY, DPAD_ARM_THICKNESS + 12, DPAD_ARM_THICKNESS);
            dpadRight = Drawing::Rectangle(dpadCenterX, hArmY, DPAD_ARM_THICKNESS + 12, DPAD_ARM_THICKNESS);

            int aX = formWidth - GRID_MARGIN - AB_BUTTON_RADIUS - 18;
            int bX = aX - (AB_BUTTON_RADIUS * 2 + 22);

            aButtonCenter = Point(aX, dpadCenterY);
            bButtonCenter = Point(bX, dpadCenterY);
            abButtonRadius = AB_BUTTON_RADIUS;
        }

        /// <summary>
        /// Paints the outer console case.
        /// </summary>
        /// <param name="g">The graphics surface to paint on.</param>
        void paintConsoleCase(Graphics^ g)
        {
            Pen^ hi = gcnew Pen(PLASTIC_HI, 2.0f);
            Pen^ lo = gcnew Pen(PLASTIC_LO, 2.0f);

            g->DrawLine(hi, 0, MENU_HEIGHT, formWidth - 1, MENU_HEIGHT);
            g->DrawLine(hi, 0, MENU_HEIGHT, 0, formHeight - 1);
            g->DrawLine(lo, formWidth - 1, MENU_HEIGHT, formWidth - 1, formHeight - 1);
            g->DrawLine(lo, 0, formHeight - 1, formWidth - 1, formHeight - 1);

            int seamY = gridTop + GridPanel::BOARD_SIZE * GridPanel::CELL_SIZE + CONTROLS_GAP - 6;
            Pen^ seamLo = gcnew Pen(PLASTIC_LO, 1.0f);
            Pen^ seamHi = gcnew Pen(PLASTIC_HI, 1.0f);
            g->DrawLine(seamLo, 8, seamY, formWidth - 8, seamY);
            g->DrawLine(seamHi, 8, seamY + 1, formWidth - 8, seamY + 1);

            delete hi; delete lo; delete seamLo; delete seamHi;
        }

        /// <summary>
        /// Paints the screen bezel around the grid.
        /// </summary>
        /// <param name="g">The graphics surface to paint on.</param>
        void paintScreenBezel(Graphics^ g)
        {
            int boardPx = GridPanel::BOARD_SIZE * GridPanel::CELL_SIZE + 2;
            int padX = 14;
            int padY = 14;

            Drawing::Rectangle bezel = Drawing::Rectangle(
                GRID_MARGIN - padX,
                gridTop - padY,
                boardPx + padX * 2,
                boardPx + padY * 2);

            SolidBrush^ darkBrush = gcnew SolidBrush(BEZEL_DK);
            fillChamferedRect(g, darkBrush, bezel, 10, 18);

            Drawing::Rectangle inner = Drawing::Rectangle(
                bezel.X + 3, bezel.Y + 3,
                bezel.Width - 6, bezel.Height - 6);
            SolidBrush^ bezelBrush = gcnew SolidBrush(BEZEL);
            fillChamferedRect(g, bezelBrush, inner, 8, 15);

            paintMatrixLabel(g, bezel);
            paintPowerLed(g, boardPx, padX);

            delete darkBrush; delete bezelBrush;
        }

        /// <summary>
        /// Paints the matrix label on the screen bezel.
        /// </summary>
        /// <param name="g">The graphics surface to paint on.</param>
        /// <param name="bezel">The bezel rectangle.</param>
        void paintMatrixLabel(Graphics^ g, Drawing::Rectangle bezel)
        {
            String^ tag = L"- LCD MATRIX -";
            Drawing::Font^ font = gcnew Drawing::Font(L"Consolas", 7, FontStyle::Bold);
            SizeF size = g->MeasureString(tag, font);
            SolidBrush^ brush = gcnew SolidBrush(Color::FromArgb(140, 150, 180));
            g->DrawString(
                tag, font, brush,
                bezel.X + (bezel.Width - size.Width) / 2,
                bezel.Bottom - size.Height - 2);
            delete font; delete brush;
        }

        /// <summary>
        /// Paints the power LED and label.
        /// </summary>
        /// <param name="g">The graphics surface to paint on.</param>
        /// <param name="boardPx">The board size in pixels.</param>
        /// <param name="padX">The horizontal bezel padding.</param>
        void paintPowerLed(Graphics^ g, int boardPx, int padX)
        {
            int ledX = GRID_MARGIN - padX + 4;
            int ledY = gridTop + boardPx / 2 - 4;

            SolidBrush^ outer = gcnew SolidBrush(Color::FromArgb(80, 0, 0));
            SolidBrush^ inner = gcnew SolidBrush(POWER_LED);
            g->FillEllipse(outer, ledX - 1, ledY - 1, 8, 8);
            g->FillEllipse(inner, ledX, ledY, 6, 6);

            Drawing::Font^ font = gcnew Drawing::Font(L"Consolas", 6, FontStyle::Bold);
            SolidBrush^ brush = gcnew SolidBrush(Color::FromArgb(100, 100, 110));
            g->DrawString(L"PWR", font, brush, (float)(ledX - 3), (float)(ledY + 8));

            delete outer; delete inner; delete font; delete brush;
        }

        /// <summary>
        /// Paints the title branding.
        /// </summary>
        /// <param name="g">The graphics surface to paint on.</param>
        void paintBranding(Graphics^ g)
        {
            Drawing::Font^ brandFont = gcnew Drawing::Font(
                L"Consolas", 18,
                (FontStyle)(FontStyle::Bold | FontStyle::Italic));
            String^ brand = L"PATHFINDER 64";
            SolidBrush^ brandBrush = gcnew SolidBrush(Color::FromArgb(40, 40, 55));
            SolidBrush^ shadowBrush = gcnew SolidBrush(Color::FromArgb(60, 0, 0, 0));

            int x = GRID_MARGIN + 6;
            int y = BRAND_TOP;
            g->DrawString(brand, brandFont, shadowBrush, (float)(x + 1), (float)(y + 1));
            g->DrawString(brand, brandFont, brandBrush, (float)x, (float)y);

            Drawing::Font^ subFont = gcnew Drawing::Font(L"Consolas", 7, FontStyle::Bold);
            SolidBrush^ subBrush = gcnew SolidBrush(Color::FromArgb(110, 110, 120));
            g->DrawString(L"8-BIT EDITION \u00B7 KELLEN & JAKE ", subFont, subBrush,
                (float)x, (float)(y + 24));

            paintBrandingChevron(g, x, y);

            delete brandFont; delete brandBrush; delete shadowBrush;
            delete subFont; delete subBrush;
        }

        /// <summary>
        /// Paints the branding chevron mark.
        /// </summary>
        /// <param name="g">The graphics surface to paint on.</param>
        /// <param name="brandX">The branding x-coordinate.</param>
        /// <param name="brandY">The branding y-coordinate.</param>
        void paintBrandingChevron(Graphics^ g, int brandX, int brandY)
        {
            Pen^ pen = gcnew Pen(Color::FromArgb(40, 40, 55), 2.0f);
            int cx = brandX - 14;
            int cy = brandY + 11;
            g->DrawLine(pen, cx, cy - 5, cx + 5, cy);
            g->DrawLine(pen, cx + 5, cy, cx, cy + 5);
            delete pen;
        }

        /// <summary>
        /// Paints the speaker grille.
        /// </summary>
        /// <param name="g">The graphics surface to paint on.</param>
        void paintSpeakerGrille(Graphics^ g)
        {
            int boardPx = GridPanel::BOARD_SIZE * GridPanel::CELL_SIZE + 2;
            int controlsRowY = gridTop + boardPx + CONTROLS_GAP;
            int baseX = formWidth - GRID_MARGIN - 70;
            int baseY = controlsRowY + CONTROLS_ROW_H - BOTTOM_BTN_H - 36;

            Pen^ slitDark = gcnew Pen(PLASTIC_LO, 2.0f);
            Pen^ slitHi = gcnew Pen(PLASTIC_HI, 1.0f);
            for (int i = 0; i < 6; i++)
            {
                int slitX = baseX + i * 8;
                g->DrawLine(slitDark, slitX, baseY, slitX + 12, baseY - 12);
                g->DrawLine(slitHi, slitX + 1, baseY + 1, slitX + 13, baseY - 11);
            }
            delete slitDark; delete slitHi;
        }

        /// <summary>
        /// Paints the D-pad and action buttons.
        /// </summary>
        /// <param name="g">The graphics surface to paint on.</param>
        void paintDPadAndButtons(Graphics^ g)
        {
            paintDPad(g);
            paintActionButtons(g);
        }

        /// <summary>
        /// Paints the directional pad.
        /// </summary>
        /// <param name="g">The graphics surface to paint on.</param>
        void paintDPad(Graphics^ g)
        {
            int boardPx = GridPanel::BOARD_SIZE * GridPanel::CELL_SIZE + 2;
            int controlsRowY = gridTop + boardPx + CONTROLS_GAP;
            int dpadCenterX = GRID_MARGIN + 16 + DPAD_ARM_LENGTH + 10;
            int dpadCenterY = controlsRowY + (CONTROLS_ROW_H - BOTTOM_BTN_H - 28) / 2;

            Drawing::Rectangle hArm = Drawing::Rectangle(
                dpadCenterX - DPAD_ARM_LENGTH - 12,
                dpadCenterY - DPAD_ARM_THICKNESS / 2,
                (DPAD_ARM_LENGTH + 12) * 2,
                DPAD_ARM_THICKNESS);
            Drawing::Rectangle vArm = Drawing::Rectangle(
                dpadCenterX - DPAD_ARM_THICKNESS / 2,
                dpadCenterY - DPAD_ARM_LENGTH - 12,
                DPAD_ARM_THICKNESS,
                (DPAD_ARM_LENGTH + 12) * 2);

            SolidBrush^ pad = gcnew SolidBrush(DPAD_DARK);
            SolidBrush^ cap = gcnew SolidBrush(DPAD_CAP);
            SolidBrush^ shade = gcnew SolidBrush(Color::FromArgb(70, 0, 0, 0));

            g->FillRectangle(shade, hArm.X + 3, hArm.Y + 4, hArm.Width, hArm.Height);
            g->FillRectangle(shade, vArm.X + 3, vArm.Y + 4, vArm.Width, vArm.Height);

            fillRoundedRect(g, pad, hArm, 7);
            fillRoundedRect(g, pad, vArm, 7);

            Drawing::Rectangle capRect = Drawing::Rectangle(
                dpadCenterX - 14, dpadCenterY - 14, 28, 28);
            g->FillRectangle(cap, capRect);

            Pen^ capPen = gcnew Pen(Color::FromArgb(20, 20, 24), 1.0f);
            g->DrawRectangle(capPen, capRect);

            paintDPadArrows(g, dpadCenterX, dpadCenterY);

            delete pad; delete cap; delete shade; delete capPen;
        }

        /// <summary>
        /// Paints the directional arrows on the D-pad.
        /// </summary>
        /// <param name="g">The graphics surface to paint on.</param>
        /// <param name="centerX">The D-pad center x-coordinate.</param>
        /// <param name="centerY">The D-pad center y-coordinate.</param>
        void paintDPadArrows(Graphics^ g, int centerX, int centerY)
        {
            SolidBrush^ tri = gcnew SolidBrush(DPAD_TRI);
            int triSize = 7;
            drawTriangle(g, tri, centerX, centerY - DPAD_ARM_LENGTH + 6, triSize, 0);
            drawTriangle(g, tri, centerX, centerY + DPAD_ARM_LENGTH - 6, triSize, 2);
            drawTriangle(g, tri, centerX - DPAD_ARM_LENGTH + 6, centerY, triSize, 3);
            drawTriangle(g, tri, centerX + DPAD_ARM_LENGTH - 6, centerY, triSize, 1);
            delete tri;
        }

        /// <summary>
        /// Paints the action buttons.
        /// </summary>
        /// <param name="g">The graphics surface to paint on.</param>
        void paintActionButtons(Graphics^ g)
        {
            paintCircleButton(g, bButtonCenter.X, bButtonCenter.Y, abButtonRadius, L"DELETE");
            paintCircleButton(g, aButtonCenter.X, aButtonCenter.Y, abButtonRadius, L"PLACE");
        }

        /// <summary>
        /// Paints a circular action button.
        /// </summary>
        /// <param name="g">The graphics surface to paint on.</param>
        /// <param name="cx">The center x-coordinate.</param>
        /// <param name="cy">The center y-coordinate.</param>
        /// <param name="r">The button radius.</param>
        /// <param name="label">The button label.</param>
        void paintCircleButton(Graphics^ g, int cx, int cy, int r, String^ label)
        {
            SolidBrush^ shade = gcnew SolidBrush(Color::FromArgb(70, 0, 0, 0));
            g->FillEllipse(shade, cx - r + 2, cy - r + 4, r * 2, r * 2);

            SolidBrush^ body = gcnew SolidBrush(BUTTON_BODY);
            g->FillEllipse(body, cx - r, cy - r, r * 2, r * 2);

            SolidBrush^ gloss = gcnew SolidBrush(Color::FromArgb(140, 230, 230, 240));
            g->FillEllipse(gloss, cx - r + 4, cy - r + 4, r, (int)(r * 0.6));

            Pen^ rim = gcnew Pen(Color::FromArgb(80, 18, 50), 1.5f);
            g->DrawEllipse(rim, cx - r, cy - r, r * 2, r * 2);

            Drawing::Font^ font = gcnew Drawing::Font(L"Consolas", 11, FontStyle::Bold);
            SolidBrush^ brush = gcnew SolidBrush(INK);
            SizeF size = g->MeasureString(label, font);
            g->DrawString(label, font, brush,
                (float)(cx - size.Width / 2),
                (float)(cy + r + 2));

            delete shade; delete body; delete gloss; delete rim;
            delete font; delete brush;
        }

        /// <summary>
        /// Paints the start and select pill controls.
        /// </summary>
        /// <param name="g">The graphics surface to paint on.</param>
        void paintStartSelectPills(Graphics^ g)
        {
            int boardPx = GridPanel::BOARD_SIZE * GridPanel::CELL_SIZE + 2;
            int controlsRowY = gridTop + boardPx + CONTROLS_GAP;
            int y = controlsRowY + CONTROLS_ROW_H - BOTTOM_BTN_H - 26;
            int cx = formWidth / 2;

            paintPill(g, cx - 40, y, 32, 9, L"SELECT");
            paintPill(g, cx + 8, y, 32, 9, L"START");
        }

        /// <summary>
        /// Paints one pill-shaped control.
        /// </summary>
        /// <param name="g">The graphics surface to paint on.</param>
        /// <param name="x">The x-coordinate.</param>
        /// <param name="y">The y-coordinate.</param>
        /// <param name="w">The width.</param>
        /// <param name="h">The height.</param>
        /// <param name="label">The pill label.</param>
        void paintPill(Graphics^ g, int x, int y, int w, int h, String^ label)
        {
            Drawing::Rectangle r = Drawing::Rectangle(x, y, w, h);

            SolidBrush^ shade = gcnew SolidBrush(Color::FromArgb(80, 0, 0, 0));
            fillRoundedRect(g, shade, Drawing::Rectangle(x + 1, y + 2, w, h), h / 2);

            SolidBrush^ body = gcnew SolidBrush(Color::FromArgb(80, 80, 90));
            fillRoundedRect(g, body, r, h / 2);

            Drawing::Font^ font = gcnew Drawing::Font(L"Consolas", 7, FontStyle::Bold);
            SolidBrush^ brush = gcnew SolidBrush(Color::FromArgb(60, 60, 70));
            SizeF size = g->MeasureString(label, font);
            g->DrawString(label, font, brush,
                (float)(x + (w - size.Width) / 2),
                (float)(y + h + 1));

            delete shade; delete body; delete font; delete brush;
        }

        /// <summary>
        /// Fills a rounded rectangle.
        /// </summary>
        /// <param name="g">The graphics surface to paint on.</param>
        /// <param name="brush">The brush used to fill the rectangle.</param>
        /// <param name="r">The rectangle to fill.</param>
        /// <param name="radius">The corner radius.</param>
        void fillRoundedRect(Graphics^ g, Brush^ brush, Drawing::Rectangle r, int radius)
        {
            if (radius <= 0)
            {
                g->FillRectangle(brush, r);
                return;
            }

            GraphicsPath^ path = gcnew GraphicsPath();
            int diameter = radius * 2;
            path->AddArc(r.X, r.Y, diameter, diameter, 180, 90);
            path->AddArc(r.Right - diameter, r.Y, diameter, diameter, 270, 90);
            path->AddArc(r.Right - diameter, r.Bottom - diameter, diameter, diameter, 0, 90);
            path->AddArc(r.X, r.Bottom - diameter, diameter, diameter, 90, 90);
            path->CloseFigure();
            g->FillPath(brush, path);
            delete path;
        }

        /// <summary>
        /// Fills a chamfered rectangle.
        /// </summary>
        /// <param name="g">The graphics surface to paint on.</param>
        /// <param name="brush">The brush used to fill the rectangle.</param>
        /// <param name="r">The rectangle to fill.</param>
        /// <param name="chamfer">The chamfer amount.</param>
        /// <param name="curve">The curve amount.</param>
        void fillChamferedRect(Graphics^ g, Brush^ brush, Drawing::Rectangle r, int chamfer, int curve)
        {
            GraphicsPath^ path = gcnew GraphicsPath();
            path->AddLine(r.X + chamfer, r.Y, r.Right - curve, r.Y);
            path->AddArc(r.Right - curve, r.Y, curve, curve, 270, 90);
            path->AddLine(r.Right, r.Y + curve, r.Right, r.Bottom - curve);
            path->AddArc(r.Right - curve, r.Bottom - curve, curve, curve, 0, 90);
            path->AddLine(r.Right - curve, r.Bottom, r.X + curve, r.Bottom);
            path->AddArc(r.X, r.Bottom - curve, curve, curve, 90, 90);
            path->AddLine(r.X, r.Bottom - curve, r.X, r.Y + chamfer);
            path->AddLine(r.X, r.Y + chamfer, r.X + chamfer, r.Y);
            path->CloseFigure();
            g->FillPath(brush, path);
            delete path;
        }

        /// <summary>
        /// Draws a directional triangle.
        /// </summary>
        /// <param name="g">The graphics surface to paint on.</param>
        /// <param name="brush">The brush used to fill the triangle.</param>
        /// <param name="cx">The center x-coordinate.</param>
        /// <param name="cy">The center y-coordinate.</param>
        /// <param name="size">The triangle size.</param>
        /// <param name="direction">The triangle direction.</param>
        void drawTriangle(Graphics^ g, Brush^ brush, int cx, int cy, int size, int direction)
        {
            array<Point>^ points = gcnew array<Point>(3);
            switch (direction)
            {
            case 0:
                points[0] = Point(cx, cy - size);
                points[1] = Point(cx - size, cy + size);
                points[2] = Point(cx + size, cy + size);
                break;
            case 1:
                points[0] = Point(cx + size, cy);
                points[1] = Point(cx - size, cy - size);
                points[2] = Point(cx - size, cy + size);
                break;
            case 2:
                points[0] = Point(cx, cy + size);
                points[1] = Point(cx - size, cy - size);
                points[2] = Point(cx + size, cy - size);
                break;
            case 3:
                points[0] = Point(cx - size, cy);
                points[1] = Point(cx + size, cy - size);
                points[2] = Point(cx + size, cy + size);
                break;
            default:
                break;
            }
            g->FillPolygon(brush, points);
        }
    };
}