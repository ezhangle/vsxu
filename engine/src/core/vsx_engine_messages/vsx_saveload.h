/**
* Project: VSXu Engine: Realtime modular visual programming engine.
*
* This file is part of Vovoid VSXu Engine.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies AB Copyright (C) 2003-2013
* @see The GNU Lesser General Public License (LGPL)
*
* VSXu Engine is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU Lesser General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/



if (cmd == "state_load")
{
  vsx_string base_path = vsx_data_path::get_instance()->data_path_get();

  vsx_string errmsg;
  state_name = c->parts[1];
  if (load_state(base_path+str_replace(";","/",base64_decode(c->parts[1])),&errmsg))
  {
    cmd_out->add_raw(vsx_string("alert_fail ")+base64_encode(c->raw)+" Error "+base64_encode("Could not load state. Error message was:|"+errmsg), VSX_COMMAND_GARBAGE_COLLECT);
  } //else
  {
    cmd_out->add_raw("clear_ok", VSX_COMMAND_GARBAGE_COLLECT);
  }
  goto process_message_queue_end;
}



if (cmd == "state_load_done")
{
  commands_out_cache.add_raw("state_load_ok "+state_name, true);
  send_state_to_client(&commands_out_cache);
  goto process_message_queue_end;
}


// deletes every single component in the whole engine
if (cmd == "clear")
{
  i_clear(&commands_out_cache);
  cmd_out->add_raw(cmd+"_ok "+cmd_data, VSX_COMMAND_GARBAGE_COLLECT);
  goto process_message_queue_end;
}



if (cmd == "meta_set") {
  meta_information = base64_decode(c->parts[1]);
  vsx_string deli("|");
  explode(meta_information, deli, meta_fields);
  goto process_message_queue_end;
}



if (cmd == "meta_get") {
  cmd_out->add_raw("meta_get_ok "+base64_encode(meta_information), VSX_COMMAND_GARBAGE_COLLECT);
  goto process_message_queue_end;
}



if (cmd == "set_silent") {
  if (c->parts[1] == "1")
  cmd_out->set_accept_commands(0);
  else
  if (c->parts[1] == "0")
  cmd_out->set_accept_commands(1);
  goto process_message_queue_end;
}



if (cmd == "package_export")
{
  if (filesystem.is_archive())
  {
    cmd_out->add_raw(vsx_string("alert_fail ")+base64_encode(c->raw)+" Error "+base64_encode("Can not save a production!"), VSX_COMMAND_GARBAGE_COLLECT);
  }
  else

  if (c->parts.size() == 3) {
    vsx_string base_path = vsx_data_path::get_instance()->data_path_get();

    vsxf tfs;
    vsx_string filename = (c->parts[2]+str_replace(";","",c->parts[1]));
    tfs.archive_create((base_path+filename).c_str());
    vsx_command_list savelist;
    get_state_as_commandlist(savelist);
    savelist.set_filesystem(&tfs);
    savelist.save_to_file("_states/_default");
    for (forge_map_iter = forge_map.begin(); forge_map_iter != forge_map.end(); ++forge_map_iter)
    {
      vsx_comp* comp = (*forge_map_iter).second;
      if (comp->component_class != "macro")
      {
        for (unsigned long i = 0; i < comp->get_params_in()->param_id_list.size(); ++i)
        {
          if (comp->get_params_in()->param_id_list[i]->module_param->type == VSX_MODULE_PARAM_ID_RESOURCE)
          {
            if (comp->get_params_in()->param_id_list[i]->get_string() != comp->get_params_in()->param_id_list[i]->get_default_string())
            {
              tfs.archive_add_file(comp->get_params_in()->param_id_list[i]->get_string(),0,0, vsx_data_path::get_instance()->data_path_get() +comp->get_params_in()->param_id_list[i]->get_string());
            }
          }
        }
        for (unsigned long i = 0; i < comp->module->resources.size(); ++i) {
          printf("engine resource add: %s\n", comp->module->resources[i].c_str() );
          tfs.archive_add_file(comp->module->resources[i],0,0,vsx_data_path::get_instance()->data_path_get()+comp->module->resources[i]);
        }
      }
    }
    cmd_out->add_raw(vsx_string(cmd+"_ok ")+c->parts[1], VSX_COMMAND_GARBAGE_COLLECT);
    tfs.archive_close();
  }
  goto process_message_queue_end;
}



if (cmd == "state_save")
{
  if (filesystem.is_archive())
  {
    cmd_out->add_raw(vsx_string("alert_fail ")+base64_encode(c->raw)+" Error "+base64_encode("Can not save a production!"), VSX_COMMAND_GARBAGE_COLLECT);
  }
  else
  if (c->parts.size() == 2)
  {
    vsx_string base_path = vsx_data_path::get_instance()->data_path_get();
    vsxf tfs;
    vsx_command_list savelist;
    get_state_as_commandlist(savelist);
    savelist.set_filesystem(&tfs);
    vsx_string filename = base_path+"states/"+str_replace(";","/",c->parts[1]);
    savelist.save_to_file(filename);
    cmd_out->add_raw(vsx_string(cmd+"_ok ")+c->parts[1], VSX_COMMAND_GARBAGE_COLLECT);

    vsx_string s2 = str_replace(" ","\\ ",c->parts[1]);
    cmd_out->add_raw("states_list "+s2, VSX_COMMAND_GARBAGE_COLLECT);
    cmd_out->add_raw("states_list_end", VSX_COMMAND_GARBAGE_COLLECT);
  }
  goto process_message_queue_end;
}
