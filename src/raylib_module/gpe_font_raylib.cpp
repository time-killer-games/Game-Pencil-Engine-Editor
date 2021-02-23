/*
gpe_font_raylib.cpp
This file is part of:
GAME PENCIL ENGINE
https://www.pawbyte.com/gamepencilengine
Copyright (c) 2014-2021 Nathan Hurde, Chase Lee.

Copyright (c) 2014-2021 PawByte LLC.
Copyright (c) 2014-2021 Game Pencil Engine contributors ( Contributors Page )

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the “Software”), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

-Game Pencil Engine <https://www.pawbyte.com/gamepencilengine>


*/


#include "gpe_font_raylib.h"

namespace gpe
{
    font_pair_raylib::font_pair_raylib(Font fontIn,std::string str_in)
    {
        lastAlphaRendered = 255;
        str = str_in;
        strTexture = NULL;
        text_width = 0;
        text_height = 0;
        if( (int)str_in.size() > 0)
        {
            //Render text surface
            Image font_img_pair = ImageTextEx( fontIn, str_in.c_str(), fontIn.baseSize, fontIn.charsPadding, WHITE );
            Texture2D font_pair_texture = LoadTextureFromImage( font_img_pair );
            UnloadImage( font_img_pair );

            text_width = font_pair_texture.width;
            text_height = font_pair_texture.height;

            strTexture = new texture_raylib();
            strTexture->change_texture( font_pair_texture );
        }
    }

    font_pair_raylib::~font_pair_raylib()
    {
        if( strTexture!=NULL)
        {
            delete strTexture;
            strTexture = NULL;
        }
    }

    texture_raylib * font_pair_raylib::get_texture()
    {
        return strTexture;
    }

    font_raylib_tff::font_raylib_tff(std::string file_loc, int f_size, bool make_monospaced, std::string f_nickname,  int id_number )
    {
        font_system_type = "raylib_ttf";
        last_used_halign = fa_left;
        last_used_valign = fa_top;
        font_id = id_number;
        mono_width = 0;
        mono_height = 0;
        number_width = 0;
        number_height = 0;
        font_is_monospaced = make_monospaced;
        raylib_held_font = LoadFontEx( file_loc.c_str(), f_size, 0, 0 );
        font_family_name = "";
        font_nickname = f_nickname;

        if( (int)file_loc.size() > 0 )
        {
            if( !main_file_url_manager->file_exists( file_loc) )
            {
                gpe::error_log->report("Font-Error: Unable to located [" + file_loc + "]!" );
            }
            else
            {
                gpe::error_log->report("Font-Update: Attempting to load font from ["+file_loc +"] file!" );
            }
        }

        int i = 0;
        if (raylib_held_font.texture.id != GetFontDefault().texture.id)
        {
            gpe::error_log->report("Font-Error: Unable to load [" + file_loc + "]!" );

        }

        font_family_name = stg_ex::int_to_string(f_size)+"pt";
        std::string longCharstring = "";
        for( i = 0; i < 256; i++)
        {
            longCharstring+=i;
        }

        font_pair_raylib * monoSpacedPair = find_texture_raylib("W");
        if( monoSpacedPair!=NULL)
        {
            number_width = mono_width = monoSpacedPair->get_width();
            number_height = mono_height = monoSpacedPair->get_height();
        }

        for( i = 0; i < 10; i++)
        {
            font_character_pairs[ stg_ex::int_to_string(i)] =  new font_pair_raylib( raylib_held_font, stg_ex::int_to_string(i) );
        }
        font_character_pairs["." ] = new font_pair_raylib( raylib_held_font,"." );
        font_character_pairs["," ] =  new font_pair_raylib( raylib_held_font,"," );
        font_character_pairs["-" ] =  new font_pair_raylib( raylib_held_font,"-" );
        font_character_pairs["(" ] =  new font_pair_raylib( raylib_held_font,"(" );
        font_character_pairs[")" ] = new font_pair_raylib( raylib_held_font,")" );

        if( make_monospaced == false)
        {
            std::map<std::string,font_pair_raylib *>::iterator foundIterator;
            foundIterator = font_character_pairs.find("9");
            if( foundIterator!= font_character_pairs.end() )
            {
                font_pair_raylib * tempfont_pair_raylib = foundIterator->second;
                if( tempfont_pair_raylib!=NULL )
                {
                    if( tempfont_pair_raylib->get_width() > number_width)
                    {
                        number_width = tempfont_pair_raylib->get_width();
                    }
                    if( tempfont_pair_raylib->get_height() > number_height)
                    {
                        number_height = tempfont_pair_raylib->get_height();
                    }
                }
            }
        }
    }

    font_raylib_tff::~font_raylib_tff()
    {
        clear_cache();
        font_pair_raylib * tempfont_pair_raylib = NULL;
        for (std::map<std::string,font_pair_raylib *>::iterator it=font_character_pairs.begin(); it!=font_character_pairs.end(); ++it)
        {
            tempfont_pair_raylib = it->second;
            if( tempfont_pair_raylib!=NULL)
            {
                delete tempfont_pair_raylib;
                tempfont_pair_raylib = NULL;
            }
        }
        font_character_pairs.clear();
        UnloadFont( raylib_held_font );
    }

    void font_raylib_tff::clear_cache()
    {
        font_pair_raylib * tempfont_pair_raylib = NULL;
        for (std::map<std::string,font_pair_raylib *>::iterator it=font_text_pairs.begin(); it!=font_text_pairs.end(); ++it)
        {
            tempfont_pair_raylib = it->second;
            if( tempfont_pair_raylib!=NULL)
            {
                delete tempfont_pair_raylib;
                tempfont_pair_raylib = NULL;
            }
        }
        font_text_pairs.clear();
    }

    font_base * font_raylib_tff::create_new(std::string file_loc, int f_size, bool make_monospaced , const std::string f_nickname , int id_number )
    {
        return new font_raylib_tff( file_loc, f_size, make_monospaced, f_nickname, id_number );
    }

    int font_raylib_tff::get_cache_count()
    {
        return (int)font_text_pairs.size();
    }

    int font_raylib_tff::get_font_id()
    {
        return font_id;
    }

    void font_raylib_tff::get_metrics(std::string text_to_render, int * width_value, int * height_value)
    {
        int tSize = (int)text_to_render.size();
        if( tSize > 0)
        {
            if( font_is_monospaced)
            {
                *width_value = mono_width*tSize;
                *height_value = mono_height;
            }
            else
            {
                font_pair_raylib *  newFoundPair = find_texture_raylib( text_to_render);
                if( newFoundPair!=NULL)
                {
                    *width_value = newFoundPair->get_width();
                    *height_value = newFoundPair->get_height();
                }

            }
        }
        else
        {
            *width_value = 0;
            *height_value = 0;
        }
    }

    void font_raylib_tff::get_numbered_metrics(std::string text_to_render, int * width_value, int * height_value)
    {
        int tSize = (int)text_to_render.size();
        if( tSize > 0)
        {
            *width_value = number_width*tSize;
            *height_value = number_height;
        }
        else
        {
            *width_value = 0;
            *height_value = 0;
        }
    }

    void font_raylib_tff::get_wrapped_string_metrics( const std::string str_in, int line_width, int linePadding, int * width_value, int *height_value )
    {
        if( (int)str_in.size() == 0 || line_width <=0 )
        {
            return;
        }
        std::vector < std::string > wrappedTextLine;
        stg_ex::wrap_string( str_in, wrappedTextLine, line_width);
        if( (int)wrappedTextLine.size() == 0  )
        {
            *width_value = 0;
            *height_value = 0;
        }
        else if( (int)wrappedTextLine.size() == 1 )
        {

        }
        else
        {

        }
    }

    Font font_raylib_tff::get_raylib_font()
    {
        return raylib_held_font;
    }

    font_pair_base * font_raylib_tff::find_character_texture( const std::string id_number )
    {
        return find_character_texture_raylib( id_number );
    }

    font_pair_raylib * font_raylib_tff::find_character_texture_raylib( const std::string id_number )
    {
        font_pair_raylib * fPair = NULL;
        texture_raylib * fraylibTexture = NULL;
        std::map<std::string,font_pair_raylib *>::iterator foundVal;
        foundVal = font_character_pairs.find( id_number );
        if( foundVal !=  font_character_pairs.end() )
        {
            fPair = foundVal->second;
            if( fPair!=NULL && fPair->texture_matches(id_number) )
            {
                return fPair;
            }
        }
        else
        {
            if( id_number!=" ")
            {
                fPair = new font_pair_raylib( raylib_held_font,id_number);
                font_character_pairs[id_number] = fPair;
                return fPair;
            }
        }
        return NULL;
    }

    font_pair_base * font_raylib_tff::find_texture( const std::string text_to_render)
    {
        return find_texture_raylib( text_to_render );
    }

    font_pair_raylib * font_raylib_tff::find_texture_raylib( const std::string text_to_render)
    {
        font_pair_raylib * fPair = NULL;
        texture_raylib * fraylibTexture = NULL;
        std::map< const std::string,font_pair_raylib *>::iterator foundVal;
        foundVal = font_text_pairs.find( text_to_render );
        if( foundVal !=  font_text_pairs.end() )
        {
            return foundVal->second;
        }
        else if( text_to_render!=" ")
        {
            fPair = new font_pair_raylib( raylib_held_font,text_to_render);
            font_text_pairs[text_to_render] = fPair;
            return fPair;
        }
        return NULL;
    }

    void font_raylib_tff::render_bitmapped_text( int x_pos, int y_pos, std::string number_to_render, color * text_color, int alignment_h,int alignment_v, int render_alpha )
    {
        int numberSize = (int)number_to_render.size();
        if( render_alpha > 0 && numberSize >0 )
        {
            switch(alignment_h)
            {
            case gpe::fa_center:
                x_pos=x_pos-(number_width*numberSize)/2;
                //last_used_halign=gpe::fa_center;
                break;

            case gpe::fa_right:
                x_pos=x_pos-(number_width*numberSize);
                // last_used_halign=gpe::fa_right;
                break;

            //rendering left will be the default
            default:
                // last_used_halign=gpe::fa_left;
                break;
            }
            switch(alignment_v)
            {
            case gpe::fa_middle:
                y_pos=y_pos-number_height/2;
                // last_used_valign=gpe::fa_middle;
                break;

            case gpe::fa_bottom:
                y_pos=y_pos-number_height;
                // last_used_valign=gpe::fa_middle;
                break;

            //rendering left will be the default
            default:
                //last_used_valign=gpe::fa_top;
                break;
            }
            font_pair_raylib * strTex = NULL;
            texture_raylib * fPairTex = NULL;
            int i = 0;
            std::string currCharacter = "";
            for( i = 0; i < numberSize; i++)
            {
                currCharacter = stg_ex::get_substring(number_to_render,i,1);
                strTex = find_character_texture_raylib( currCharacter );
                if( strTex!=NULL)
                {
                    fPairTex = strTex->get_texture();
                    if( render_alpha > 255)
                    {
                        render_alpha = 255;
                    }
                    if( fPairTex!=NULL && render_alpha >0)
                    {
                        int texWid = 0;
                        int texHeight =0;
                        //raylib_SetTextureColorMod( fPairTex, text_color->get_r(), text_color->get_g(), text_color->get_b() );

                        if( strTex->lastAlphaRendered!=render_alpha )
                        {
                            //raylib_SetTextureAlphaMod(fPairTex,render_alpha);
                            strTex->lastAlphaRendered = render_alpha;
                        }
                        //Get image dimensions
                        texWid = strTex->get_width();
                        texHeight = strTex->get_height();

                        if(alignment_h<gpe::fa_left || alignment_h<0)
                        {
                            alignment_h=last_used_halign;
                        }
                        if(alignment_v <gpe::fa_top || alignment_v<0)
                        {
                            alignment_v=last_used_valign;
                        }
                        //raylib_Rect clip = {x_pos, y_pos, texWid,texHeight};
                        //raylib_RenderCopy( renderer_main_raylib->get_gpe_renderer_raylib(),fPairTex, NULL, &clip);
                        //raylib_SetTextureColorMod( fPairTex, c_white->get_r(), c_white->get_g(), c_white->get_b() );
                    }
                }
                x_pos+=number_width;
            }
        }
    }

    void font_raylib_tff::render_text( int x_pos, int y_pos, std::string text_to_render, color * text_color, int alignment_h,int alignment_v,int render_alpha )
    {
        if( render_alpha > 0 && text_color!=NULL)
        {
            //raylib_SetRenderDrawColor( renderer_main_raylib->get_gpe_renderer_raylib(), text_color->get_r(), text_color->get_g(), text_color->get_b(), 255 );
            font_pair_raylib * strTex = find_texture_raylib(text_to_render);
            texture_raylib * fPairTex=  NULL;
            if( strTex!=NULL)
            {
                fPairTex = strTex->get_texture();

                if( fPairTex!=NULL && render_alpha >0)
                {
                    if( render_alpha >= 255)
                    {
                        render_alpha = 255;
                    }
                    int texWid = 0;
                    int texHeight =0;
                    //raylib_SetTextureColorMod( fPairTex, text_color->get_r(), text_color->get_g(), text_color->get_b() );

                    if( strTex->lastAlphaRendered!=render_alpha )
                    {
                        //raylib_SetTextureAlphaMod(fPairTex,render_alpha);
                        strTex->lastAlphaRendered = render_alpha;
                    }
                    //Get image dimensions
                    texWid = strTex->get_width();
                    texHeight = strTex->get_height();

                    if(alignment_h<gpe::fa_left || alignment_h > gpe::fa_right )
                    {
                        alignment_h=last_used_halign;
                    }

                    if(alignment_v < gpe::fa_top || alignment_v > gpe::fa_bottom )
                    {
                        alignment_v=last_used_valign;
                    }
                    switch(alignment_h)
                    {
                    case gpe::fa_center:
                        x_pos=x_pos-texWid/2;
                        //last_used_halign=gpe::fa_center;
                        break;

                    case gpe::fa_right:
                        x_pos=x_pos-texWid;
                        // last_used_halign=gpe::fa_right;
                        break;

                    //rendering left will be the default
                    default:
                        // last_used_halign=gpe::fa_left;
                        break;
                    }
                    switch(alignment_v)
                    {
                        case gpe::fa_middle:
                            y_pos=y_pos-texHeight/2;
                            // last_used_valign=gpe::fa_middle;
                        break;

                        case gpe::fa_bottom:
                            y_pos=y_pos-texHeight;
                            // last_used_valign=gpe::fa_middle;
                        break;

                        //rendering left will be the default
                        default:
                            //last_used_valign=gpe::fa_top;
                        break;
                    }
                    /*
                    raylib_Rect clip = {x_pos, y_pos, texWid,texHeight};
                    raylib_RenderCopy( renderer_main_raylib->get_gpe_renderer_raylib(),fPairTex, NULL, &clip);
                    //raylib_SetTextureColorMod( fPairTex, c_white->get_r(), c_white->get_g(), c_white->get_b() );
                    */
                }
                else
                {
                    gpe::error_log->report("Unable to find raylib_text for ["+text_to_render+"]...");
                }
            }
            else
            {
                gpe::error_log->report("Unable to render ["+text_to_render+"]...");
            }
        }
        else
        {
            gpe::error_log->report("Unable to render text  ["+text_to_render+"]...");
        }
    }

    void font_raylib_tff::render_text_scaled( int x_pos, int y_pos, std::string text_to_render, color * text_color, float text_scale, int alignment_h,int alignment_v, int render_alpha )
    {

    }

    void font_raylib_tff::render_text_resized( int x_pos, int y_pos, std::string text_to_render, color * text_color, int alignment_h,int alignment_v, int render_width, int render_height, int render_alpha )
    {
        if( render_alpha > 0)
        {
            font_pair_raylib * strTex = find_texture_raylib(text_to_render);
            if( strTex!=NULL)
            {
                texture_raylib * fPairTex = strTex->get_texture();
                if( fPairTex!=NULL)
                {
                    int texWid = 0;
                    int texHeight =0;
                    //raylib_SetTextureColorMod( fPairTex, text_color->get_r(), text_color->get_g(), text_color->get_b() );
                    if( strTex->lastAlphaRendered!=render_alpha )
                    {
                        //raylib_SetTextureAlphaMod(fPairTex,render_alpha);
                        strTex->lastAlphaRendered = render_alpha;
                    }
                    //Get image dimensions
                    texWid = strTex->get_width();
                    texHeight = strTex->get_height();

                    //Get image dimensions
                    if( render_width<=0 || render_width > strTex->get_width())
                    {
                        texWid = strTex->get_width();
                    }
                    else
                    {
                        texWid = render_width;
                    }

                    if( render_height<=0 || render_height > strTex->get_height() )
                    {
                        texHeight = strTex->get_height();
                    }
                    else
                    {
                        texHeight = render_height;
                    }

                    if(alignment_h<gpe::fa_left || alignment_h > gpe::fa_right )
                    {
                        alignment_h=last_used_halign;
                    }

                    if(alignment_v < gpe::fa_top || alignment_v > gpe::fa_bottom )
                    {
                        alignment_v=last_used_valign;
                    }
                    switch(alignment_h)
                    {
                    case gpe::fa_center:
                        x_pos=x_pos-texWid/2;
                        //last_used_halign=gpe::fa_center;
                        break;

                    case gpe::fa_right:
                        x_pos=x_pos-texWid;
                        // last_used_halign=gpe::fa_right;
                        break;

                    //rendering left will be the default
                    default:
                        // last_used_halign=gpe::fa_left;
                        break;
                    }
                    switch(alignment_v)
                    {
                    case gpe::fa_middle:
                        y_pos=y_pos-texHeight/2;
                        // last_used_valign=gpe::fa_middle;
                        break;

                    case gpe::fa_bottom:
                        y_pos=y_pos-texHeight;
                        // last_used_valign=gpe::fa_middle;
                        break;

                    //rendering left will be the default
                    default:
                        //last_used_valign=gpe::fa_top;
                        break;
                    }

                    /*
                    raylib_Rect clip = {x_pos, y_pos, texWid,texHeight};
                    raylib_Rect render_rect = {0, 0, texWid, texHeight};
                    raylib_RenderCopy( renderer_main_raylib->get_gpe_renderer_raylib(),fPairTex, &render_rect, &clip);
                    //raylib_SetTextureColorMod( fPairTex, c_white->get_r(), c_white->get_g(), c_white->get_b() );
                    */
                }
            }
        }
    }

    void font_raylib_tff::render_text_boxed( int x_pos, int y_pos, std::string text_to_render, color * text_color,color * boxColor,int alignment_h,int alignment_v, int render_alpha )
    {
        if( render_alpha > 0)
        {
            font_pair_raylib * strTex = find_texture_raylib(text_to_render);
            texture_raylib * fPairTex=  NULL;
            if( strTex!=NULL)
            {
                fPairTex = strTex->get_texture();
                if( render_alpha > 255)
                {
                    render_alpha = 255;
                }
                if( fPairTex!=NULL && render_alpha >0)
                {
                    int texWid = 0;
                    int texHeight =0;
                    //raylib_SetTextureColorMod( fPairTex, text_color->get_r(), text_color->get_g(), text_color->get_b() );

                    if( strTex->lastAlphaRendered!=render_alpha )
                    {
                        //raylib_SetTextureAlphaMod(fPairTex,render_alpha);
                        strTex->lastAlphaRendered = render_alpha;
                    }
                    //Get image dimensions
                    texWid = strTex->get_width();
                    texHeight = strTex->get_height();

                    if(alignment_h<gpe::fa_left || alignment_h > gpe::fa_right )
                    {
                        alignment_h=last_used_halign;
                    }

                    if(alignment_v < gpe::fa_top || alignment_v > gpe::fa_bottom )
                    {
                        alignment_v=last_used_valign;
                    }
                    switch(alignment_h)
                    {
                    case gpe::fa_center:
                        x_pos=x_pos-texWid/2;
                        //last_used_halign=gpe::fa_center;
                        break;

                    case gpe::fa_right:
                        x_pos=x_pos-texWid;
                        // last_used_halign=gpe::fa_right;
                        break;

                    //rendering left will be the default
                    default:
                        // last_used_halign=gpe::fa_left;
                        break;
                    }
                    switch(alignment_v)
                    {
                    case gpe::fa_middle:
                        y_pos=y_pos-texHeight/2;
                        // last_used_valign=gpe::fa_middle;
                        break;

                    case gpe::fa_bottom:
                        y_pos=y_pos-texHeight;
                        // last_used_valign=gpe::fa_middle;
                        break;

                    //rendering left will be the default
                    default:
                        //last_used_valign=gpe::fa_top;
                        break;
                    }
                    gpe::shape_rect gpeClip;
                    gpeClip.x = x_pos;
                    gpeClip.y = y_pos;
                    gpeClip.w = texWid;
                    gpeClip.h = texHeight;
                    //raylib_Rect raylibClip = {x_pos, y_pos, texWid,texHeight};
                    gpe::gcanvas->render_rect(  &gpeClip,boxColor,false,render_alpha );
                    //raylib_RenderCopy( renderer_main_raylib->get_gpe_renderer_raylib(),fPairTex, NULL, &raylibClip);
                    //raylib_SetTextureColorMod( fPairTex, c_white->get_r(), c_white->get_g(), c_white->get_b() );
                }
            }
        }
    }

    void font_raylib_tff::render_text_rotated( int x_pos, int y_pos, std::string text_to_render, color * text_color, float textAngle, int render_alpha )
    {
        if( render_alpha > 0)
        {
            font_pair_raylib * strTex = find_texture_raylib(text_to_render);
            texture_raylib * fPairTex=  NULL;
            if( strTex!=NULL)
            {
                fPairTex = strTex->get_texture();
                if( render_alpha > 255)
                {
                    render_alpha = 255;
                }
                if( fPairTex!=NULL && render_alpha >0)
                {
                    int texWid = 0;
                    int texHeight =0;
                    //raylib_SetTextureColorMod( fPairTex, text_color->get_r(), text_color->get_g(), text_color->get_b() );

                    if( strTex->lastAlphaRendered!=render_alpha )
                    {
                        //raylib_SetTextureAlphaMod(fPairTex,render_alpha);
                        strTex->lastAlphaRendered = render_alpha;
                    }
                    //Get image dimensions
                    texWid = strTex->get_width();
                    texHeight = strTex->get_height();

                    /*
                    raylib_Rect raylibDstrect = {x_pos-texWid, y_pos-texHeight, texWid,texHeight};
                    raylib_Point raylibCenterPoint = {texWid, texHeight};
                    raylib_RenderCopyEx( renderer_main_raylib->get_gpe_renderer_raylib(),fPairTex, NULL,&raylibDstrect,textAngle,&raylibCenterPoint, raylib_FLIP_NONE );
                    //raylib_SetTextureColorMod( fPairTex, c_white->get_r(), c_white->get_g(), c_white->get_b() );
                    */
                }
            }
        }
    }

    bool font_raylib_tff::render_text_special( int x_pos, int y_pos, std::string text_to_render, color * text_color, int alignment_h,int alignment_v, float render_angle , float render_scale, int render_alpha )
    {
        if( render_alpha > 0)
        {
            font_pair_raylib * strTex = find_texture_raylib(text_to_render);
            if( strTex!=NULL)
            {
                texture_raylib * fPairTex = strTex->get_texture();
                if( fPairTex!=NULL)
                {
                    int texWid = 0;
                    int texHeight =0;
                    //raylib_SetTextureColorMod( fPairTex, text_color->get_r(), text_color->get_g(), text_color->get_b() );
                    if( strTex->lastAlphaRendered!=render_alpha )
                    {
                        strTex->lastAlphaRendered = render_alpha;
                    }
                    //Get image dimensions
                    texWid = (float)strTex->get_width() * render_scale;
                    texHeight = (float)strTex->get_height() * render_scale;

                    //Get image dimensions
                    if( texWid < 0)
                    {
                        texWid *=-1;
                        render_angle *=-1;
                    }

                    if( texHeight<0 )
                    {
                        texHeight *=-1;
                        render_angle *=-1;
                    }

                    if(alignment_h<gpe::fa_left || alignment_h > gpe::fa_right )
                    {
                        alignment_h=last_used_halign;
                    }

                    if(alignment_v < gpe::fa_top || alignment_v > gpe::fa_bottom )
                    {
                        alignment_v=last_used_valign;
                    }
                    switch(alignment_h)
                    {
                    case gpe::fa_center:
                        x_pos=x_pos-texWid/2;
                        //last_used_halign=gpe::fa_center;
                        break;

                    case gpe::fa_right:
                        x_pos=x_pos-texWid;
                        // last_used_halign=gpe::fa_right;
                        break;

                    //rendering left will be the default
                    default:
                        // last_used_halign=gpe::fa_left;
                        break;
                    }
                    switch(alignment_v)
                    {
                    case gpe::fa_middle:
                        y_pos=y_pos-texHeight/2;
                        // last_used_valign=gpe::fa_middle;
                        break;

                    case gpe::fa_bottom:
                        y_pos=y_pos-texHeight;
                        // last_used_valign=gpe::fa_middle;
                        break;

                    //rendering left will be the default
                    default:
                        //last_used_valign=gpe::fa_top;
                        break;
                    }


                    return true;
                }
            }
        }
        return false;
    }

    bool init_raylib_font_system()
    {
        //If we already started the font system do nothing.
        if( gfs!=NULL )
        {
            delete gfs;
            gfs = NULL;
        }


        gfs = new font_system_controller();
        gfs->font_template = new font_raylib_tff("",0,false,"",-1 );
        gpe::error_log->report("raylib_module font system started...");
        return true;
    }

    void quit_raylib_font_system()
    {
        gpe::error_log->report("Quitting raylib_module fony system....");

        if( gfs!=NULL )
        {
            delete gfs;
            gfs = NULL;
        }
        gfs = new font_system_controller();
        gpe::error_log->report("raylib_module font system quit...");
    }
}