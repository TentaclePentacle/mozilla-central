/* -*- Mode: Java; c-basic-offset: 4; tab-width: 20; indent-tabs-mode: nil; -*-
 * ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is Mozilla Android code.
 *
 * The Initial Developer of the Original Code is Mozilla Foundation.
 * Portions created by the Initial Developer are Copyright (C) 2011
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Kartikaya Gupta <kgupta@mozilla.com>
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

package org.mozilla.gecko.gfx;

import android.graphics.Rect;
import android.graphics.RectF;
import org.json.JSONException;
import org.json.JSONObject;

public final class RectUtils {
    public static Rect create(JSONObject json) {
        try {
            int x = json.getInt("x");
            int y = json.getInt("y");
            int width = json.getInt("width");
            int height = json.getInt("height");
            return new Rect(x, y, x + width, y + height);
        } catch (JSONException e) {
            throw new RuntimeException(e);
        }
    }

    public static Rect contract(Rect rect, int lessWidth, int lessHeight) {
        float halfLessWidth = (float)lessWidth / 2.0f;
        float halfLessHeight = (float)lessHeight / 2.0f;
        return new Rect((int)Math.round((float)rect.left + halfLessWidth),
                        (int)Math.round((float)rect.top + halfLessHeight),
                        (int)Math.round((float)rect.right - halfLessWidth),
                        (int)Math.round((float)rect.bottom - halfLessHeight));
    }

    public static RectF contract(RectF rect, float lessWidth, float lessHeight) {
        float halfLessWidth = lessWidth / 2;
        float halfLessHeight = lessHeight / 2;
        return new RectF(rect.left + halfLessWidth,
                         rect.top + halfLessHeight,
                         rect.right - halfLessWidth,
                         rect.bottom - halfLessHeight);
    }

    public static RectF expand(RectF rect, float moreWidth, float moreHeight) {
        float halfMoreWidth = moreWidth / 2;
        float halfMoreHeight = moreHeight / 2;
        return new RectF(rect.left - halfMoreWidth,
                         rect.top - halfMoreHeight,
                         rect.right + halfMoreWidth,
                         rect.bottom + halfMoreHeight);
    }

    public static RectF intersect(RectF one, RectF two) {
        float left = Math.max(one.left, two.left);
        float top = Math.max(one.top, two.top);
        float right = Math.min(one.right, two.right);
        float bottom = Math.min(one.bottom, two.bottom);
        return new RectF(left, top, Math.max(right, left), Math.max(bottom, top));
    }

    public static RectF scale(RectF rect, float scale) {
        float x = rect.left * scale;
        float y = rect.top * scale;
        return new RectF(x, y,
                         x + (rect.width() * scale),
                         y + (rect.height() * scale));
    }

    public static Rect round(RectF rect) {
        return new Rect(Math.round(rect.left), Math.round(rect.top),
                        Math.round(rect.right), Math.round(rect.bottom));
    }

    public static IntSize getSize(Rect rect) {
        return new IntSize(rect.width(), rect.height());
    }

    /* Returns a new RectF which restricts a source rect to the area inside a second destination rect.
     * If the source rect is wider/taller than the destination rect, it's width/height will be shortened
     * (and its aspect ratio will NOT be maintained).
    */
    public static RectF restrict(RectF rect, RectF dest) {
        float width = Math.min(rect.width(), dest.width());
        float height = Math.min(rect.height(), dest.height());
        float x = Math.max(dest.left, Math.min(dest.right-width, rect.left));
        float y = Math.max(dest.top, Math.min(dest.bottom-height, rect.top));
        return new RectF(x, y, x+width, y+height);
    }
}
