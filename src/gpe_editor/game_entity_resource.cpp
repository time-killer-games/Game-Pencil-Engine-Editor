/*
game_entity_resource.cpp
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

#include "game_entity_resource.h"

objectResParentChainLink::objectResParentChainLink( int origObjectType)
{
    chainObjectType = origObjectType;
}

objectResParentChainLink::~objectResParentChainLink()
{
    heldChains.clear();
}

void objectResParentChainLink::grab_chain( objectResParentChainLink * chainIn )
{
    if( chainIn!=NULL)
    {
        heldChains.push_back( chainIn->chainObjectType );
        for( int i = 0; i < (int)chainIn->heldChains.size(); i++)
        {
            heldChains.push_back( chainIn->heldChains[i] );
        }
    }
}

bool objectResParentChainLink::is_inchain(int chainId)
{
    for( int i = 0; i < (int)heldChains.size(); i++)
    {
        if( heldChains[i]==chainId)
        {
            return true;
        }
    }
    return false;
}

gameEntityResource::gameEntityResource(pawgui::widget_resource_container * pFolder)
{
    resourcePostProcessed = true;
    projectParentFolder = pFolder;
    imageIndex = 0;
    editorMode = 0;
    int i = 0;

    animationIndex = -1;
    animInEditor = NULL;

    if(saveResource_button!=NULL)
    {
        saveResource_button->disable_self();
    }



    headerCodeArea = new pawgui::widget_text_editor( true);
    headerCodeArea->set_read_only(  );
    sourceCodeArea = new pawgui::widget_text_editor(true);
    sourceCodeArea->set_read_only(  );

    objModeSelector = new pawgui::widget_selectbox("Entity Functions");
    objModeSelector->add_option("Components",OBJ_MODE_COMPONENTS,pawgui::rsm_gui->texture_add("componentsIcon", gpe::app_directory_name+"resources/gfx/iconpacks/fontawesome/tags.png"),NULL,0,false,true);
    objModeSelector->add_option("Header Code(C++)",OBJ_MODE_HEADER,pawgui::rsm_gui->texture_add("superScriptIoon", gpe::app_directory_name+"resources/gfx/iconpacks/fontawesome/superscript.png"),NULL,0,false,true);
    objModeSelector->add_option("Source Code",OBJ_MODE_SOURCE,pawgui::rsm_gui->texture_add("codeIcon", gpe::app_directory_name+"resources/gfx/iconpacks/fontawesome/code.png"),NULL,0,false,true);

    if( projectParentFolder!=NULL )
    {
        animationField = new pawgui::widget_drop_down_resource_menu( "Default Animation",projectParentFolder->find_resource_from_name(gpe::resource_type_names_plural[ gpe::resource_type_animation]) );
        loadResource_button->set_width(animationField->get_width() );
        confirmResource_button->set_width(animationField->get_width() );
        parentObjectField = new pawgui::widget_drop_down_resource_menu( "Parent Entity",projectParentFolder->find_resource_from_name(gpe::resource_type_names_plural[ gpe::resource_type_object] ) );
    }
    else
    {
        animationField = NULL;
        parentObjectField = NULL;
    }

    customComponentsGuiList = new pawgui::widget_panel_list();
    customComponentsGuiList->panelAlignType = pawgui::panel_align_left;
    componentsMainGuiLabel = new pawgui::widget_label_text ("Custom Components","Custom components, useful for scene editor");
    componentsGuiLabel = new pawgui::widget_label_text ("Custom Components","Custom components, useful for scene editor");
    inheritParentComponent_button = new pawgui::widget_button_push( gpe::app_directory_name+"resources/gfx/iconpacks/fontawesome/users.png","Inherit Components","Inherit components from parent object class");
    resetComponents_button = new pawgui::widget_button_push( gpe::app_directory_name+"resources/gfx/iconpacks/fontawesome/trash.png","Reset Components","Clears all components form this object class");
    addNewComponentDropDown = new pawgui::widget_dropdown_menu( "Add Component",false);
    setup_object_components ( addNewComponentDropDown );
    addNewComponentDropDown->set_width(224);

    removeComponent_button = new pawgui::widget_button_icon( gpe::app_directory_name+"resources/gfx/iconpacks/fontawesome/minus.png","Remove Component");
    editCompnent_button = new pawgui::widget_button_icon( gpe::app_directory_name+"resources/gfx/iconpacks/fontawesome/edit.png","Edit Component");
    component_settings_button = new pawgui::widget_button_icon( gpe::app_directory_name+"resources/gfx/iconpacks/fontawesome/trash.png","Component Settings");

    checkBoxIsVisible = new pawgui::widget_checkbox("Visible","The actor is rendered on screen", true);
    checkBoxNeedsCamera = new pawgui::widget_checkbox("Needs Camera","Entity will only perform logic if in view of any camera [Highly recommended for most objects]", true);
    checkBoxIsMoveable = new pawgui::widget_checkbox("Mobile","Uses engine default physics", true);
    checkBoxIsContinuous = new pawgui::widget_checkbox("Continuous","Entity continues throughout game scenes[Keep unchecked for most objects]", false);
    resource_nameLabel = new pawgui::widget_label_title("Entity","");
}

gameEntityResource::~gameEntityResource()
{
    if( animationField!=NULL)
    {
        delete animationField;
        animationField = NULL;
    }
    if( resource_nameLabel!=NULL)
    {
        delete resource_nameLabel;
        resource_nameLabel = NULL;
    }
    if( parentObjectField!=NULL)
    {
        delete parentObjectField;
        parentObjectField= NULL;
    }

    if( objModeSelector!=NULL)
    {
        delete objModeSelector;
        objModeSelector = NULL;
    }

    if( checkBoxNeedsCamera!=NULL)
    {
        delete checkBoxNeedsCamera;
        checkBoxNeedsCamera = NULL;
    }
    if( checkBoxIsMoveable!=NULL)
    {
        delete checkBoxIsMoveable;
        checkBoxIsMoveable = NULL;
    }
    if( checkBoxIsVisible!=NULL)
    {
        delete checkBoxIsVisible;
        checkBoxIsVisible = NULL;
    }
    if( checkBoxIsContinuous!=NULL)
    {
        delete checkBoxIsContinuous;
        checkBoxIsContinuous = NULL;
    }
    int i = 0;

    if( headerCodeArea!=NULL)
    {
        delete headerCodeArea;
        headerCodeArea = NULL;
    }

    if( sourceCodeArea!=NULL)
    {
        delete sourceCodeArea;
        sourceCodeArea = NULL;
    }


    //Deletes the components
    GPE_ObjectComponent * tempComponent = NULL;
    for( i = (int)customComponentRealList.size()-1; i>=0; i--)
    {
        tempComponent = customComponentRealList[i];
        if( tempComponent!=NULL )
        {
            delete tempComponent;
            tempComponent = NULL;
            customComponentRealList.erase(customComponentRealList.begin()+i);
        }
    }
    customComponentRealList.clear();

    //Component stuffs
    if( componentsMainGuiLabel!=NULL)
    {
        delete componentsMainGuiLabel;
        componentsMainGuiLabel = NULL;
    }
    if( componentsGuiLabel!=NULL)
    {
        delete componentsGuiLabel;
        componentsGuiLabel = NULL;
    }
    if( customComponentsGuiList!=NULL)
    {
        delete customComponentsGuiList;
        customComponentsGuiList = NULL;
    }
    if( inheritParentComponent_button!=NULL)
    {
        delete inheritParentComponent_button;
        inheritParentComponent_button = NULL;
    }
    if( resetComponents_button!=NULL)
    {
        delete resetComponents_button;
        resetComponents_button = NULL;
    }
    if( addNewComponentDropDown!=NULL)
    {
        delete addNewComponentDropDown;
        addNewComponentDropDown = NULL;
    }
    if( removeComponent_button!=NULL)
    {
        delete removeComponent_button;
        removeComponent_button = NULL;
    }
    if( editCompnent_button!=NULL)
    {
        delete editCompnent_button;
        editCompnent_button = NULL;
    }
    if( component_settings_button!=NULL)
    {
        delete component_settings_button;
        component_settings_button = NULL;
    }
}

void gameEntityResource::add_component(std::string newComponentData)
{
    pawgui::widget_basic * newComponentMeat = add_gui_component(newComponentData);
    if( newComponentMeat!=NULL)
    {
        GPE_ObjectComponent * tempComponent = NULL;
        for( int iComponents = 0; iComponents < (int)customComponentRealList.size(); iComponents++)
        {
            tempComponent = customComponentRealList.at(iComponents);
            if( tempComponent!=NULL)
            {
                if( tempComponent->get_name()==newComponentMeat->get_name() )
                {
                    //component already exists so delete it and return
                    delete newComponentMeat;
                    newComponentMeat = NULL;
                    return;
                }
            }
        }
        GPE_ObjectComponent * newComponent= new GPE_ObjectComponent();
        if( newComponent!=NULL)
        {
            newComponent->set_component( newComponentMeat);
            customComponentRealList.push_back(newComponent);
        }
        else if( newComponentMeat!=NULL)
        {
            delete newComponentMeat;
            newComponentMeat = NULL;
        }
    }
}

bool gameEntityResource::build_intohtml5_file(std::ofstream * fileTarget, int leftTabAmount)
{
    bool buildSuccessful = true;
    if( fileTarget!=NULL && fileTarget->is_open() )
    {
        int iErrorLine = 0;
        std::string nestedTabsStr = pawgui::generate_tabs( leftTabAmount  );
        std::string nestedTabsStrObjFunc = pawgui::generate_tabs( leftTabAmount +1 );

        //*fileTarget << nestedTabsStr << "var " << get_name() << " = " << exportBuildGlobalId << ";\n";
        *fileTarget << nestedTabsStr << "GPE._obj_" << get_name() << " =  (function (x_pos_in, y_pos_in,object_layer_id) \n";
        *fileTarget << nestedTabsStr << "{ \n";
        *fileTarget << nestedTabsStrObjFunc << "function obj_" << get_name() <<" (x_pos_in, y_pos_in,object_layer_id) \n";
        *fileTarget << nestedTabsStrObjFunc << "{ \n";
        pawgui::widget_resource_container * parentRes = parentObjectField->get_selected_container();
        if( parentRes!=NULL)
        {
            *fileTarget << nestedTabsStrObjFunc << "    GPE._obj_" << parentRes->get_name() <<".call(this,x_pos_in, y_pos_in,object_layer_id); \n";
        }
        else
        {
            *fileTarget << nestedTabsStrObjFunc << "    GPE.game_object.call(this,x_pos_in, y_pos_in,object_layer_id); \n";
        }
        *fileTarget << nestedTabsStrObjFunc << "    this.gpeObjectType ="<< exportBuildGlobalId << "; \n";
        pawgui::widget_resource_container * animationRes = animationField->get_selected_container();
        if( animationRes!=NULL)
        {
            *fileTarget << nestedTabsStrObjFunc << "    this.init_animation(" << animationRes->exportBuildGlobalId << ");\n";
        }
        else
        {
            *fileTarget << nestedTabsStrObjFunc << "    this.init_animation(-1);\n";
        }

        if( checkBoxIsVisible!=NULL)
        {
            *fileTarget << nestedTabsStrObjFunc << "    this.is_visible = " << checkBoxIsVisible->is_clicked() << ";\n";
        }
        else
        {
            *fileTarget << nestedTabsStrObjFunc << "    this.is_visible = false;\n";
        }
        //
        if( checkBoxIsContinuous!=NULL)
        {
            *fileTarget << nestedTabsStrObjFunc << "    this.gpeIsContinuousObject = " << checkBoxIsContinuous->is_clicked() << ";\n";
        }
        else
        {
            *fileTarget << nestedTabsStrObjFunc << "    this.gpeIsContinuousObject = false;\n";
        }
        //
        if( checkBoxNeedsCamera!=NULL)
        {
            *fileTarget << nestedTabsStrObjFunc << "    this.isViewIndependent = " << !checkBoxNeedsCamera->is_clicked() << ";\n";
        }
        else
        {
            *fileTarget << nestedTabsStrObjFunc << "    this.isViewIndependent = false;\n";
        }

        if( !sourceCodeArea->compile_into_code( fileTarget, leftTabAmount+2,true,true) )
        {
            for( iErrorLine=0; iErrorLine < (int)sourceCodeArea->foundSyntaxErrors.size(); iErrorLine++)
            {
                main_editor_log->log_build_error(get_name()+"'s SRC: "+sourceCodeArea->foundSyntaxErrors[iErrorLine] );
                buildSuccessful = false;
            }
        }
        //returns object and gives it its prototype(parent object)
        if( parentRes!=NULL)
        {
            *fileTarget << nestedTabsStrObjFunc << "_obj_" << get_name() << ".prototype = new GPE._obj_" << parentRes->get_name() <<"(x_pos_in, y_pos_in); \n";
        }
        else
        {
            *fileTarget << nestedTabsStrObjFunc << "_obj_" << get_name() << ".prototype = new GPE.game_object(x_pos_in, y_pos_in); \n";
        }
        *fileTarget << nestedTabsStrObjFunc << "return obj_" << get_name() <<"; \n";
        *fileTarget << nestedTabsStr << "}());\n\n";
    }
    return buildSuccessful;
}

bool gameEntityResource::build_intocpp_file(std::ofstream * fileTarget, int leftTabAmount  )
{
    return true;
}

void gameEntityResource::compile_cpp()
{

}

pawgui::widget_resource_container * gameEntityResource::get_parent_resource()
{
    if( parentObjectField!=NULL)
    {
        if( parentObjectField->get_selected_id()< 0 || parentObjectField->get_selected_id()==get_global_rid() )
        {
            parentObjectField->set_name("Parent Object");
            parentObjectField->set_selection( -1);
            return NULL;
        }
        return parentObjectField->get_selected_container();
    }
    return NULL;
}

gpe::animaton2d * gameEntityResource::get_resource_animation()
{
    if( projectParentFolder!=NULL)
    {
        pawgui::widget_resource_container * allanimationsFolder = projectParentFolder->find_resource_from_name(gpe::resource_type_names_plural[ gpe::resource_type_animation]);
        if( allanimationsFolder!=NULL)
        {
            pawgui::widget_resource_container *objTypeContainer = allanimationsFolder->find_resource_from_id( animationIndex);
            if( objTypeContainer!=NULL)
            {
                return objTypeContainer->get_resource_animation();
            }
        }
    }
    return NULL;
}

bool gameEntityResource::include_local_files( std::string pBuildDir , int buildType )
{
    return true;
}

bool gameEntityResource::is_build_ready()
{
    recentErrorMessage = "";
    return true;
}

void gameEntityResource::integrate_into_syntax()
{
    if( current_project!=NULL)
    {
        current_project->add_project_keyword(resource_name,"Project "+gpe::resource_type_names[ gpe::resource_type_object]+" Resource",-1,"");

    }
}

void gameEntityResource::manage_components( gpe::shape_rect * view_space, gpe::shape_rect * cam )
{
    view_space = gpe::camera_find(view_space);
    cam = gpe::camera_find(cam);
    if(cam!=NULL && view_space!=NULL && customComponentsGuiList!=NULL && renameBox!=NULL)
    {
        customComponentsGuiList->set_coords(0, 0);
        customComponentsGuiList->set_width(view_space->w );
        customComponentsGuiList->set_height(view_space->h  );
        customComponentsGuiList->barXPadding = pawgui::padding_default;
        customComponentsGuiList->barXMargin = 0;

        customComponentsGuiList->clear_list();
        //Start of New Code
        customComponentsGuiList->add_gui_element(renameBox,true);
        customComponentsGuiList->add_gui_element(animationField,true);
        customComponentsGuiList->add_gui_element(parentObjectField,true);
        //customComponentsGuiList->add_gui_element(loadResource_button,true);
        customComponentsGuiList->add_gui_element(checkBoxNeedsCamera,true);
        customComponentsGuiList->add_gui_element(checkBoxIsMoveable,true);
        customComponentsGuiList->add_gui_element(checkBoxIsVisible,true);
        customComponentsGuiList->add_gui_element(checkBoxIsContinuous,true);
        //End of New Code

        customComponentsGuiList->add_gui_element(componentsMainGuiLabel,true);
        customComponentsGuiList->add_gui_element(resetComponents_button,true);
        customComponentsGuiList->add_gui_element(inheritParentComponent_button,true);
        customComponentsGuiList->add_gui_element(addNewComponentDropDown,true);

        customComponentsGuiList->add_gui_element(componentsGuiLabel,true);

        GPE_ObjectComponent * tempCustomComponent = NULL;
        GPE_ObjectComponent * otherTempCustomComponent = NULL;
        int iRealCompo = 0;
        int jRealCompo = 0;

        for(  iRealCompo = 0; iRealCompo < (int)customComponentRealList.size(); iRealCompo++)
        {
            tempCustomComponent = customComponentRealList.at(iRealCompo);
            if( tempCustomComponent!=NULL)
            {
                customComponentsGuiList->add_gui_element(tempCustomComponent->get_component(),false);
                customComponentsGuiList->add_gui_element(tempCustomComponent->get_gear(),true);
            }
        }
        customComponentsGuiList->process_self(view_space,cam);

        for(  iRealCompo = (int)customComponentRealList.size()-1; iRealCompo>=0; iRealCompo--)
        {
            tempCustomComponent = customComponentRealList.at(iRealCompo);
            if( tempCustomComponent!=NULL)
            {
                if(tempCustomComponent->get_gear()!=NULL)
                {
                    if( tempCustomComponent->get_gear()->is_clicked() )
                    {
                        customComponentsGuiList->clear_list();
                        pawgui::context_menu_open(-1,-1,256);
                        pawgui::main_context_menu->add_menu_option("Rename Component",0,NULL,-1,NULL,true,true);
                        pawgui::main_context_menu->add_menu_option("Delete Component",1,NULL,-1,NULL,false,true);
                        if( tempCustomComponent->get_type()=="dropdown" || tempCustomComponent->get_type()=="selectbox")
                        {
                            pawgui::main_context_menu->add_menu_option("Add Option",2,NULL,-1,NULL,false,true);
                            pawgui::main_context_menu->add_menu_option("Remove Option",3,NULL,-1,NULL,false,true);
                        }
                        pawgui::main_context_menu->add_menu_option("Exit Menu",4,NULL,-1,NULL,true,true);
                        int menuSelection = pawgui::context_menu_process();
                        if( menuSelection==0)
                        {
                            //rename
                            std::string newComponentName = pawgui::get_string_from_popup("Name your new component","Must be unique!","");
                            bool componentNameInUseAlready = false;
                            if( newComponentName.size() > 0 )
                            {
                                if( tempCustomComponent->get_type()=="labeltext" || stg_ex::is_alnum(newComponentName,false,true) )
                                {
                                    for(  jRealCompo = 0; jRealCompo < (int)customComponentRealList.size(); jRealCompo++)
                                    {
                                        if( jRealCompo!=iRealCompo)
                                        {
                                            otherTempCustomComponent = customComponentRealList.at(jRealCompo);
                                            if( otherTempCustomComponent!=NULL)
                                            {
                                                if( otherTempCustomComponent->get_name() ==newComponentName )
                                                {
                                                    componentNameInUseAlready = true;
                                                }
                                            }
                                        }
                                    }
                                    if( !componentNameInUseAlready)
                                    {
                                        tempCustomComponent->rename_component( newComponentName );
                                    }
                                }
                            }
                        }
                        else if( menuSelection==1)
                        {
                            if( pawgui::display_prompt_message("[WARNING]Object Component deletion?","Are you sure you will like to delete the ["+tempCustomComponent->get_name()+"] component? This action is irreversible!")==pawgui::display_query_yes)
                            {
                                //delete
                                delete tempCustomComponent;
                                tempCustomComponent = NULL;
                                customComponentRealList.erase( customComponentRealList.begin()+iRealCompo);
                            }
                        }
                        else if( menuSelection==2)
                        {
                            if( tempCustomComponent->get_type()=="dropdown" || tempCustomComponent->get_type()=="selectbox")
                            {
                                std::string extraDatastring = "[menu]";
                                std::string newMenuOptionName = pawgui::get_string_from_popup("Enter an option name.","option name:","");
                                while( newMenuOptionName.size() > 0 && (  stg_ex::is_alnum(newMenuOptionName,true,true) ) )
                                {
                                    extraDatastring+="[option]"+newMenuOptionName+":"+newMenuOptionName+":-1[/option]";
                                    newMenuOptionName = pawgui::get_string_from_popup("Enter an option name.","option name:","");
                                }
                                extraDatastring+="[/menu]0,0";
                                tempCustomComponent->load_data(extraDatastring);
                            }
                        }
                        else if( menuSelection==3)
                        {
                            if( tempCustomComponent->get_type()=="dropdown" || tempCustomComponent->get_type()=="selectbox")
                            {
                                std::string extraDatastring = "";
                                std::string newMenuOptionName = pawgui::get_string_from_popup("Enter option to remove.","option name:","");
                                if( newMenuOptionName.size() > 0 && (  stg_ex::is_alnum(newMenuOptionName,true,true) ) )
                                {
                                    extraDatastring+="[option]"+newMenuOptionName+"[/option]";
                                    tempCustomComponent->remove_data(extraDatastring);
                                }
                            }
                        }
                        //manage_components(view_space, cam);
                        break;
                    }
                }
            }
        }
    }
}

void gameEntityResource::open_code(int lineNumb, int colNumb, std::string codeTitle)
{
    if( (int)codeTitle.size() > 0 )
    {
        pawgui::widget_text_editor * foundTextArea = NULL;

        if( codeTitle == "header" )
        {
            foundTextArea = headerCodeArea;
            objModeSelector->set_selection( OBJ_MODE_HEADER);
        }
        else
        {
            foundTextArea = sourceCodeArea;
            objModeSelector->set_selection( OBJ_MODE_SOURCE);
        }

        if( foundTextArea!=NULL && foundTextArea->has_content() )
        {
            foundTextArea->scroll_to_pos(lineNumb, colNumb);
        }
    }
}

void gameEntityResource::prerender_self(  )
{
    standardEditableGameResource::prerender_self();

    if( checkBoxNeedsCamera!=NULL)
    {
        checkBoxNeedsCamera->prerender_self( );
    }
    if( checkBoxIsMoveable!=NULL)
    {
        checkBoxIsMoveable->prerender_self( );
    }
    if( checkBoxIsVisible!=NULL)
    {
        checkBoxIsVisible->prerender_self( );
    }
    if( checkBoxIsContinuous!=NULL)
    {
        checkBoxIsContinuous->prerender_self( );
    }
}

void gameEntityResource::load_resource(std::string file_path)
{
    if( resourcePostProcessed == false  || gpe::main_file_url_manager->file_exists(file_path) )
    {
        if( main_gpe_splash_page != NULL )
        {
            main_gpe_splash_page->update_submessages( "Processing Game Object", resource_name );
        }

        std::string otherColContainerName = "";

        std::string newameObjFilename = "";
        std::string headerFileName ="";
        std::string sourceFileName ="";

        std::string soughtDir = stg_ex::file_to_dir(parentProjectName)+"/gpe_project/resources/entities/";
        std::string soughtCodeDir = stg_ex::file_to_dir(parentProjectName)+"/gpe_project/source/";
        if( gpe::main_file_url_manager->file_exists(file_path) )
        {
            newameObjFilename = file_path;
            soughtDir = stg_ex::get_path_from_file(newameObjFilename);
        }
        else
        {
            newameObjFilename = soughtDir + resource_name+".gpf";
        }

        headerFileName = soughtCodeDir + "entity_" + resource_name+".h";
        sourceFileName = soughtCodeDir + "entity_" + resource_name+".src";

        if( current_project !=NULL)
        {
            if( current_project->get_project_language_id() ==
               pawgui::program_language_cpp )
            {
                sourceFileName = soughtDir+  "entity_" + resource_name+".cpp";
            }
        }

        std::ifstream gameObjFileIn( newameObjFilename.c_str() );

        //Used for importing older object class embedded in same file...
        bool inCodeSection = false; //Default to false so nothing happens til a function is entered
        std::string nestedFunctionStr = pawgui::generate_tabs( 1  );
        std::string nestedContentStr = pawgui::generate_tabs( 2  );
        //End of variables used to pre-format older JS format embedded in same file...

        //If the level file could be loaded
        if( !gameObjFileIn.fail() )
        {
            //makes sure the file is open
            if (gameObjFileIn.is_open())
            {
                int equalPos = 0;
                int colonSeperaterPos = 0;
                std::string firstChar="";
                std::string section="";
                std::string key_string="";
                std::string valstring="";
                std::string subValstring="";
                std::string foundReturnTypestring="";
                std::string foundParametersstring ="";
                std::string currLine="";
                std::string currLineToBeProcessed;


                float foundFileVersion = 0;
                pawgui::widget_text_editor * textAreaToLoad = sourceCodeArea;
                if( current_project!=NULL )
                {
                    if( current_project->get_project_language_id() == pawgui::program_language_js )
                    {

                    }
                }

                while ( gameObjFileIn.good() )
                {
                    getline (gameObjFileIn,currLine); //gets the next line of the file
                    currLineToBeProcessed = stg_ex::trim_left_inplace(currLine);
                    currLineToBeProcessed = stg_ex::trim_right_inplace(currLineToBeProcessed);

                    if( foundFileVersion <=0)
                    {
                        //Empty Line skipping is only allowed at the top of the file
                        if(!currLineToBeProcessed.empty() )
                        {
                            //Comment skipping is only allowed at the top of the file
                            if( currLineToBeProcessed[0]!= '#' && currLineToBeProcessed[0]!='/'  )
                            {
                                //searches for an equal character and parses through the variable
                                equalPos=currLineToBeProcessed.find_first_of("=");
                                if(equalPos!=(int)std::string::npos)
                                {
                                    //if the equalPos is present, then parse on through and carryon
                                    key_string = currLineToBeProcessed.substr(0,equalPos);
                                    valstring = currLineToBeProcessed.substr(equalPos+1,currLineToBeProcessed.length());
                                    if( key_string=="Version")
                                    {
                                        foundFileVersion = stg_ex::string_to_float(valstring);
                                    }
                                }
                            }
                        }
                    }
                    else if( foundFileVersion <= 2)
                    {
                        //Begin processing the file.
                        if(!currLineToBeProcessed.empty() )
                        {
                            equalPos=currLineToBeProcessed.find_first_of("=");
                            if(equalPos!=(int)std::string::npos)
                            {
                                //if the equalPos is present, then parse on through and carryon
                                key_string = currLineToBeProcessed.substr(0,equalPos);
                                valstring = currLineToBeProcessed.substr(equalPos+1,currLineToBeProcessed.length());

                                //Seeks for new textArea to edit based on older format
                                if( stg_ex::string_starts( key_string, "[[[") )
                                {
                                    if( inCodeSection )
                                    {
                                        textAreaToLoad->add_line(nestedFunctionStr + "}");
                                        textAreaToLoad->add_line("");
                                    }

                                    if( key_string=="[[[Function")
                                    {
                                        valstring = stg_ex::split_first_string(valstring,']');
                                        if( textAreaToLoad!=NULL)
                                        {
                                            textAreaToLoad->add_line(nestedFunctionStr + "this."+valstring + " = function()" );
                                            textAreaToLoad->add_line(nestedFunctionStr + "{");
                                        }
                                        inCodeSection = true;
                                    }
                                    else if( key_string=="[[[TimedFunction")
                                    {
                                        colonSeperaterPos=valstring.find_first_of(":");
                                        if(colonSeperaterPos!=(int)std::string::npos)
                                        {
                                            subValstring = valstring.substr(colonSeperaterPos+1);
                                            subValstring = stg_ex::split_first_string(subValstring,']');
                                            valstring = valstring.substr(0,colonSeperaterPos);
                                        }
                                        else
                                        {
                                            valstring = stg_ex::split_first_string(valstring,']');
                                        }
                                        if( textAreaToLoad!=NULL)
                                        {
                                            textAreaToLoad->add_line(nestedFunctionStr + "this."+valstring  + " = function()" );
                                            textAreaToLoad->add_line(nestedFunctionStr + "{");
                                        }
                                        inCodeSection = true;
                                    }
                                    else if( key_string=="[[[CollisionFunction" || key_string=="[[[CollideFunction")
                                    {
                                        valstring = stg_ex::split_first_string(valstring,']');
                                        if( textAreaToLoad!=NULL)
                                        {
                                            textAreaToLoad->add_line(nestedFunctionStr + "this.collide_with_"+valstring  + " = function()" );
                                            textAreaToLoad->add_line(nestedFunctionStr + "{");
                                        }
                                        inCodeSection = true;
                                    }
                                    else if( key_string=="[[[SpecialFunction")
                                    {
                                        valstring = stg_ex::split_first_string(valstring,']');
                                        if( textAreaToLoad!=NULL)
                                        {
                                            textAreaToLoad->add_line(nestedFunctionStr + "this."+valstring  + " = function()" );
                                            textAreaToLoad->add_line(nestedFunctionStr + "{");
                                        }
                                        inCodeSection = true;
                                    }
                                    else if( key_string=="[[[CustomFunction")
                                    {
                                        //option 5
                                        subValstring = stg_ex::split_first_string(valstring,',');

                                        foundReturnTypestring = stg_ex::split_first_string(valstring,',');

                                        foundParametersstring = stg_ex::split_first_string(valstring,']');

                                        textAreaToLoad->add_line(nestedFunctionStr + "this."+subValstring  + " = function( "+ foundParametersstring +")" );

                                        textAreaToLoad->add_line(nestedFunctionStr + "{");
                                        inCodeSection = true;
                                    }
                                }
                                //loads the rest of the file if all text areas are closed at this poin
                                else if( key_string=="ResourceName")
                                {
                                        renameBox->set_string(valstring);
                                }
                                else if( key_string=="animation_id" || key_string=="Animation" || key_string=="animationId" )
                                {
                                    animationIndex = -1;
                                    if( animationField!=NULL)
                                    {
                                        animationField->set_selected_target(valstring);
                                        animationIndex = animationField->get_selected_id();
                                    }
                                }
                                else if( key_string=="ParentGameObjectId")
                                {
                                    parentObjectField->set_selected_target( valstring );
                                    parentObjectId = parentObjectField->get_selected_id();

                                }
                                else if( key_string=="IsContinuous")
                                {
                                    checkBoxIsContinuous->set_checked( stg_ex::string_to_bool(valstring) );
                                }
                                else if( key_string=="IsMoveable")
                                {
                                    checkBoxIsMoveable->set_checked( stg_ex::string_to_bool(valstring) );
                                }
                                else if( key_string=="IsVisible")
                                {
                                    checkBoxIsVisible->set_checked( stg_ex::string_to_bool(valstring) );
                                }
                                else if( key_string=="NeedsCamera")
                                {
                                    checkBoxNeedsCamera->set_checked( stg_ex::string_to_bool(valstring) );
                                }
                                else if( key_string=="CustomComponent")
                                {
                                    add_component(valstring);
                                }
                                else if( inCodeSection && textAreaToLoad!=NULL )
                                {
                                    textAreaToLoad->add_line( nestedContentStr + currLineToBeProcessed );
                                }
                            }
                            else if( inCodeSection && textAreaToLoad!=NULL )
                            {
                                textAreaToLoad->add_line( nestedContentStr + currLineToBeProcessed );
                            }
                        }
                    }
                    else
                    {
                        gpe::error_log->report("Invalid FoundFileVersion ="+ stg_ex::float_to_string(foundFileVersion)+".");
                    }
                }

                gameObjFileIn.close();

                //Used to pre-format older GPE end-of-file content
                if( inCodeSection )
                {
                    textAreaToLoad->add_line(nestedFunctionStr+ "}");
                    textAreaToLoad->add_line("");
                }
            }
        }

        headerCodeArea->import_text( headerFileName );
        headerCodeArea->init_save_history();

        sourceCodeArea->import_text( sourceFileName );
        sourceCodeArea->init_save_history();

        resourcePostProcessed = true;
    }
}

void gameEntityResource::process_self( gpe::shape_rect * view_space, gpe::shape_rect * cam)
{
    //Processes the object's rename box 1st
    if(resource_nameLabel!=NULL)
    {
        if( renameBox!=NULL && renameBox->get_string()!=resource_name)
        {
            resource_nameLabel->set_name(renameBox->get_string()+"*");
        }
        else
        {
            resource_nameLabel->set_name(resource_name);
        }
    }

    view_space = gpe::camera_find(view_space);
    cam = gpe::camera_find(cam);
    load_resource();

    //Exits if we have a a bad variable
    if( cam == NULL || view_space == NULL || objModeSelector == NULL )
    {
        return;
    }
    selectedMode = objModeSelector->get_selected_value();

    pawgui::widget_text_editor *  fTextArea = NULL;

    if( panel_main_editor!=NULL )
    {
        panel_main_editor->add_gui_element(resource_nameLabel,true);
        panel_main_editor->add_gui_element(objModeSelector,true);
        panel_main_editor->add_gui_element(confirmResource_button,false);
        //gpe::error_log->report("Attempting to process with code section found "+ stg_ex::int_to_string(codeSection) );
        panel_main_editor->process_self(NULL, NULL);
    }
    else
    {
        confirmResource_button->set_clicked( false );
    }

    if( confirmResource_button->is_clicked() || ( gpe::input->check_kb_down(kb_ctrl) && gpe::input->check_kb_released(kb_s) ) )
    {
        save_resource();
    }

        //
    if( selectedMode == OBJ_MODE_COMPONENTS )
    {
        manage_components(view_space, cam);
        if( loadResource_button!=NULL)
        {
            if( loadResource_button->is_clicked() )
            {

            }
        }
        if(renameBox!=NULL)
        {
            if( renameBox->get_string()!=resource_name)
            {
                isModified = true;
            }
        }

        if( parentObjectField!=NULL)
        {
            if(parentObjectField->get_name()!="Parent Object" && parentObjectField->just_activated() )
            {
                //parentObjectField->set_name("Parent Object");
                //parentObjectField->set_selection( -1);
            }
            if( parentObjectField->get_selected_id()< 0 || parentObjectField->get_selected_id()==get_global_rid() )
            {
                parentObjectField->set_name("Parent Object");
                parentObjectField->set_selection( -1);
            }
        }


        if( inheritParentComponent_button!=NULL && parentObjectField!=NULL)
        {
            if( inheritParentComponent_button->is_clicked() )
            {
                pawgui::widget_resource_container * objectParent = parentObjectField->get_selected_container();
                std::string inheritedComponentData = "";
                pawgui::widget_basic * inheritedComponent = NULL;
                bool componentAlreadyExists = false;

                if( objectParent!=NULL)
                {
                    pawgui::general_resource * parentGenRes = objectParent->get_held_resource();
                    if( parentGenRes!=NULL)
                    {
                        gameEntityResource* parentObjRes = (gameEntityResource*)parentGenRes;
                        if( parentObjRes!=NULL)
                        {
                            int jCompo = 0;
                            GPE_ObjectComponent * myTempCustomComponent = NULL;
                            GPE_ObjectComponent * parentTempCustomComponent = NULL;
                            for( int iCompo = 0; iCompo < (int)parentObjRes->customComponentRealList.size(); iCompo++)
                            {
                                parentTempCustomComponent = parentObjRes->customComponentRealList.at(iCompo);
                                componentAlreadyExists = false;
                                if( parentTempCustomComponent!=NULL)
                                {
                                    for( jCompo = 0; jCompo < (int)customComponentRealList.size(); jCompo++)
                                    {
                                        myTempCustomComponent = customComponentRealList.at(jCompo);
                                        if( myTempCustomComponent!=NULL)
                                        {
                                            if(myTempCustomComponent->get_name()==parentTempCustomComponent->get_name() )
                                            {
                                                componentAlreadyExists = true;
                                            }
                                        }
                                    }

                                    if( !componentAlreadyExists && parentTempCustomComponent->get_component()!=NULL)
                                    {
                                        //Copy Parent Component Property to Child...

                                        inheritedComponentData = parentTempCustomComponent->get_component()->get_data();
                                        inheritedComponent = add_gui_component(inheritedComponentData);
                                        if( inheritedComponent!=NULL )
                                        {
                                            GPE_ObjectComponent * newComponent = new GPE_ObjectComponent();
                                            if( newComponent!=NULL)
                                            {
                                                newComponent->set_component( inheritedComponent );
                                                customComponentRealList.push_back(newComponent);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        if( addNewComponentDropDown!=NULL)
        {
            std::string newComponentType = addNewComponentDropDown->get_selected_tag();
            if( (int)newComponentType.size() > 0)
            {
                std::string newComponentName = pawgui::get_string_from_popup("Name your new component","Must be unique!","");
                bool componentNameInUseAlready = false;
                if( newComponentName.size() > 0 )
                {
                    if( newComponentType=="labeltext" || stg_ex::is_alnum(newComponentName,false,true) )
                    {
                        GPE_ObjectComponent * myTempCustomComponent = NULL;
                        for( int iCompo = 0; iCompo < (int)customComponentRealList.size(); iCompo++)
                        {
                            myTempCustomComponent = customComponentRealList.at(iCompo);
                            if( myTempCustomComponent!=NULL)
                            {
                                if( myTempCustomComponent->get_name()==newComponentName)
                                {
                                    componentNameInUseAlready = true;
                                    break;
                                }
                            }
                        }
                        if( !componentNameInUseAlready)
                        {
                            std::string newComponentData = newComponentType+":"+newComponentName+"==|||==";

                            std::string extraDatastring = "_blank";
                            if( newComponentType=="texturl")
                            {
                                std::string newurlLink = pawgui::get_string_from_popup("Please enter a URL for this new component","Example: http://www.pawbyte.com","");
                                std::string newUrlDescription = pawgui::get_string_from_popup("Please enter a description of your URL","Please describe your URL","");
                                if( (int)newurlLink.size() > 0)
                                {
                                    int fileColonPos = newurlLink.find("file://");
                                    int httpPos = newurlLink.find("http://");
                                    int httpsPos = newurlLink.find("https://");
                                    if( fileColonPos==(int)std::string::npos && httpPos==(int)std::string::npos && httpsPos==(int)std::string::npos )
                                    {
                                        newurlLink = "http://"+newurlLink;
                                    }
                                }
                                extraDatastring = newUrlDescription+",,,"+newurlLink+",,,";
                            }
                            else if( newComponentType=="dropdown" || newComponentType=="radio")
                            {
                                extraDatastring = "[menu]";
                                std::string newMenuOptionName = pawgui::get_string_from_popup("Enter an option name.","option name:","");
                                while( (int)newMenuOptionName.size() > 0 && (  stg_ex::is_alnum(newMenuOptionName,true,true) ) )
                                {
                                    extraDatastring+="[option]"+newMenuOptionName+":"+newMenuOptionName+":-1[/option]";
                                    newMenuOptionName = pawgui::get_string_from_popup("Enter an option name.","option name:","");
                                }
                                extraDatastring+="[/menu]0,0";
                            }
                            else
                            {
                                int dropdownResPos = newComponentType.find("-resourcedropdown");
                                if( dropdownResPos!=(int)std::string::npos)
                                {
                                    std::string newDropDownType = stg_ex::get_substring(newComponentType,0,dropdownResPos);
                                    newComponentData = "resourcedropdown:"+newComponentName+"==|||==";
                                    extraDatastring = newDropDownType+",,,-1,";
                                }
                            }
                            newComponentData = newComponentData+extraDatastring;
                            pawgui::widget_basic * newComponentField = add_gui_component(newComponentData);

                            if( newComponentField!=NULL)
                            {
                                GPE_ObjectComponent * newComponent = new GPE_ObjectComponent();
                                if( newComponent!=NULL)
                                {
                                    newComponent->set_component( newComponentField );
                                    customComponentRealList.push_back(newComponent);
                                }
                            }
                        }
                        else
                        {
                            pawgui::display_user_alert("Invalid Component Name Entered","Please enter a unique component name with alpha-numeric values and no spaces!");
                        }
                    }
                    else
                    {
                        pawgui::display_user_alert("Invalid Component Name Entered","Please enter a unique component name with alpha-numeric values and no spaces!");
                    }
                }
                addNewComponentDropDown->set_option_value(-1);
            }
        }

        if( resetComponents_button!=NULL && resetComponents_button->is_clicked() && (int)customComponentRealList.size() > 0)
        {
            if( pawgui::display_prompt_message("Are you sure you want to delete all component elements?","There is no undo!")==pawgui::display_query_yes)
            {
                GPE_ObjectComponent * myTempCustomComponent = NULL;
                for( int iCompo = 0; iCompo < (int)customComponentRealList.size(); iCompo++)
                {
                    myTempCustomComponent = customComponentRealList.at(iCompo);
                    if( myTempCustomComponent!=NULL)
                    {
                        delete myTempCustomComponent;
                        myTempCustomComponent = NULL;
                    }
                }
                customComponentRealList.clear();
                customComponentsGuiList->clear_list();
            }
        }
    }
    else
    {
        if(selectedMode == OBJ_MODE_HEADER)
        {
            fTextArea = headerCodeArea;
        }
        else
        {
            fTextArea = sourceCodeArea;
        }

        if( fTextArea!=NULL)
        {
            fTextArea->set_coords(0,0);
            fTextArea->set_width(view_space->w );
            fTextArea->set_height(view_space->h);
            /*if( basicFunctionsSelector->is_clicked() )
            {
                fTextArea->isInUse;
            }*/
            fTextArea->process_self(view_space,cam);
        }
    }
}

void gameEntityResource::render_self( gpe::shape_rect * view_space, gpe::shape_rect * cam )
{
    view_space = gpe::camera_find(view_space);
    cam = gpe::camera_find(cam);
    if(cam!=NULL && view_space!=NULL )
    {
        if( selectedMode == OBJ_MODE_COMPONENTS )
        {
            if( customComponentsGuiList!=NULL)
            {
                customComponentsGuiList->render_self( view_space,cam);
            }
        }
        else
        {
            pawgui::widget_text_editor *  fTextArea = NULL;

            if( selectedMode == OBJ_MODE_HEADER )
            {
                fTextArea = headerCodeArea;
            }
            else
            {
                fTextArea = sourceCodeArea;
            }

            if(fTextArea!=NULL)
            {
                fTextArea->render_self( view_space,cam);
            }
        }
    }
}


void gameEntityResource::save_resource(std::string file_path, int backupId)
{
    if( main_gpe_splash_page != NULL )
    {
        main_gpe_splash_page->update_submessages( "Saving Game Object", resource_name );
    }
    bool usingAltSaveSource = false;
    std::string newFileOut ="";
    std::string headerFileName ="";
    std::string sourceFileName ="";
    std::string soughtDir = stg_ex::get_path_from_file(file_path);
    if( gpe::main_file_url_manager->path_exists(soughtDir) )
    {
        newFileOut = file_path;
        headerFileName = soughtDir+resource_name+".h";
        sourceFileName = soughtDir+resource_name+".src";

        if( current_project !=NULL)
        {
            if( current_project->get_project_language_id() == pawgui::program_language_cpp )
            {
                sourceFileName = soughtDir+resource_name+".cpp";
            }
        }
        usingAltSaveSource= true;
    }
    else
    {
        soughtDir = stg_ex::file_to_dir(parentProjectName)+"/gpe_project/resources/objects/";
        newFileOut = soughtDir + resource_name+".gpf";

        headerFileName = soughtDir + resource_name+".h";
        sourceFileName = soughtDir + resource_name+".src";
        if( current_project !=NULL)
        {
            if( current_project->get_project_language_id() == pawgui::program_language_cpp )
            {
                sourceFileName = soughtDir+resource_name+".cpp";
            }
        }
    }

    if( !usingAltSaveSource )
    {
        load_resource();
    }
    int i = 0;
    std::string otherColContainerName = "";
    animationIndex = -1;
    if( animationField!=NULL)
    {
        animationIndex = animationField->get_selected_id();
    }

    std::ofstream newGameObjFile( newFileOut.c_str() );
    if( !newGameObjFile.fail() )
    {
        resource_nameLabel->set_name(resource_name );
        //makes sure the file is open
        if (newGameObjFile.is_open())
        {
            write_header_on_file(&newGameObjFile);
            if( animationField!=NULL)
            {
                newGameObjFile << "animation_id=" << animationField->get_selected_name() << "\n";
            }
            if( parentObjectField!=NULL)
            {
                newGameObjFile << "ParentGameObjectId=" << parentObjectField->get_selected_name() << "\n";
            }
            if( checkBoxNeedsCamera!=NULL)
            {
                newGameObjFile << "NeedsCamera=" << checkBoxNeedsCamera->is_clicked() << "\n";
            }
            if( checkBoxIsMoveable!=NULL)
            {
                newGameObjFile << "IsMoveable=" << checkBoxIsMoveable->is_clicked() << "\n";
            }
            if( checkBoxIsVisible!=NULL)
            {
                newGameObjFile << "IsVisible=" << checkBoxIsVisible->is_clicked() << "\n";
            }
            if( checkBoxIsContinuous!=NULL)
            {
                newGameObjFile << "IsContinuous=" << checkBoxIsContinuous->is_clicked() << "\n";
            }

            //Saves the custom components
            newGameObjFile << "\n";
            GPE_ObjectComponent * tempComponentVariable = NULL;
            for( i = 0; i < (int)customComponentRealList.size(); i++)
            {
                tempComponentVariable = customComponentRealList[i];
                if( tempComponentVariable!=NULL)
                {
                    if( tempComponentVariable->get_component()!=NULL)
                    {
                        newGameObjFile << "CustomComponent="+tempComponentVariable->get_component()->get_data()+"\n";
                    }
                }
            }
            newGameObjFile.close();
        }
    }

    //Saves the object's code
    if( headerCodeArea->has_content() )
    {
        headerCodeArea->export_text( headerFileName);
    }

    if( sourceCodeArea->has_content() )
    {
        sourceCodeArea->export_text( sourceFileName );
    }

    if( !usingAltSaveSource)
    {
        isModified = false;
    }
}

int gameEntityResource::search_for_string(std::string needle)
{
    int foundstrings = 0;
    main_editor_log->log_general_comment("Searching ["+resource_name+"] object..");


    if( pawgui::main_anchor_controller!=NULL  )
    {
        pawgui::main_anchor_controller->searchResultProjectName = parentProjectName;
        pawgui::main_anchor_controller->searchResultResourceId = globalResouceIdNumber;
        pawgui::main_anchor_controller->searchResultResourceName = resource_name;
        if( headerCodeArea!=NULL && headerCodeArea->has_content() )
        {
            foundstrings+=headerCodeArea->find_all_strings(needle,pawgui::main_search_controller->findMatchCase->is_clicked(),true, "header" );
        }

        if( sourceCodeArea!=NULL && sourceCodeArea->has_content() )
        {
            foundstrings += sourceCodeArea->find_all_strings(needle,pawgui::main_search_controller->findMatchCase->is_clicked(),true, "source" );
        }
    }
    return foundstrings;
}

int gameEntityResource::search_and_replace_string(std::string needle, std::string newStr )
{
    int foundstrings = 0;
    int tempFoundCount = 0;
    main_editor_log->log_general_comment("Searching ["+resource_name+"] object..");

    if( pawgui::main_anchor_controller!=NULL )
    {
        pawgui::main_anchor_controller->searchResultProjectName = parentProjectName;
        pawgui::main_anchor_controller->searchResultResourceId = globalResouceIdNumber;
        pawgui::main_anchor_controller->searchResultResourceName = resource_name;

        if( headerCodeArea!=NULL && headerCodeArea->has_content() )
        {
            tempFoundCount =headerCodeArea->find_all_strings(needle,pawgui::main_search_controller->findMatchCase->is_clicked(),false,"header" );
            if( tempFoundCount > 0)
            {
                foundstrings+=tempFoundCount;
                //displayMessagestring = "Replaced "+ stg_ex::int_to_string( headerCodeArea->replace_all_found(needle, newStr ) )+" copies.";                        }
            }
        }

        if( sourceCodeArea!=NULL && sourceCodeArea->has_content() )
        {
            tempFoundCount = sourceCodeArea->find_all_strings(needle,pawgui::main_search_controller->findMatchCase->is_clicked(),false, "source" );
            if( tempFoundCount > 0)
            {
                foundstrings+=tempFoundCount;
                //displayMessagestring = "Replaced "+ stg_ex::int_to_string( headerCodeArea->replace_all_found(needle, newStr ) )+" copies.";                        }
            }
        }
    }
    return foundstrings;
}


void gameEntityResource::update_box(int x_new, int y_new, int newW, int newH)
{

}

bool gameEntityResource::write_data_into_projectfile(std::ofstream * fileTarget, int nestedFoldersIn)
{
    resource_nameLabel->set_name(renameBox->get_string() );
    if( fileTarget!=NULL)
    {
        if( fileTarget->is_open() )
        {
            load_resource();
            std::string nestedTabsStr = pawgui::generate_tabs( nestedFoldersIn );
            *fileTarget << nestedTabsStr << "GameObject=" << resource_name << "," << get_global_rid() << ",\n";
            return true;
        }
    }
    return false;
}

//This function is also shared by scene editor to make sure code is uniform and reduce potential mismatchings
void setup_object_components( pawgui::widget_dropdown_menu * componentMenu)
{
    if( componentMenu == NULL)
    {
        return;
    }
    componentMenu->add_menu_option("Check-Box","checkbox",1,false);
    componentMenu->add_menu_option("Color-Picker","colorpicker",2,false);
    componentMenu->add_menu_option("Drop-Down","dropdown",3,false);
    componentMenu->add_menu_option("Input-Text","inputtext",4,false);
    componentMenu->add_menu_option("Input-Number","inputnumber",5,false);
    componentMenu->add_menu_option("Radio _button","radio",6,false);
    componentMenu->add_menu_option("Text Label","labeltext",7,false);
    componentMenu->add_menu_option("Text URL","texturl",8,false);

    gpe::key_pair *  contentSelectorKey = componentMenu->add_menu_option("Content Selector","contentselector",9,false);
    contentSelectorKey->add_keypair("Content Selector-Id","contentselector-int",10);
    contentSelectorKey->add_keypair("Content Selector-float","contentselector-float",11);
    contentSelectorKey->add_keypair("Content Selector-string","contentselector-string",12);
    contentSelectorKey->add_keypair("Content Selector-Color","contentselector-color",13);

    componentMenu->add_menu_option("Texture","Textures-resourcedropdown",50,false);
    componentMenu->add_menu_option("Tilesheet","Tilesheets-resourcedropdown",51,false);
    componentMenu->add_menu_option("Animation","Animations-resourcedropdown",52,false);
    componentMenu->add_menu_option("Audio","Audio-resourcedropdown",53,false);
    componentMenu->add_menu_option("Video","Videos-resourcedropdown",54,false);
    componentMenu->add_menu_option("Function","Functions-resourcedropdown",55,false);
    componentMenu->add_menu_option("Entity","Entities-resourcedropdown",56,false);
    componentMenu->add_menu_option("Scene","Scenes-resourcedropdown",57,false);
    componentMenu->add_menu_option("Font","Fonts-resourcedropdown",58,false);
}
