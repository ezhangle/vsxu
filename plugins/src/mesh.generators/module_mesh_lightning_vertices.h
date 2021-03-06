/**
* Project: VSXu: Realtime modular visual programming engine.
*
* This file is part of Vovoid VSXu.
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


#include <vsx_rand.h>

class module_mesh_lightning_vertices : public vsx_module
{
public:
  // in
  vsx_module_param_float* rand_seed;
  vsx_module_param_float* length;
  vsx_module_param_float* lifetime_mod;
  vsx_module_param_float* num_points;
  vsx_module_param_float3* scaling;
  vsx_module_param_mesh* mesh_a;

  // out
  vsx_module_param_mesh* result;

  // internal
  vsx_mesh<>* mesh;
  vsx_vector3<> old_scaling;
  bool first_run;
  unsigned long id_a;
  unsigned long id_b;
  unsigned long lifetime;
  vsx_vector3<> delta;
  vsx_vector3<> start;
  vsx_rand rand;

  void module_info(vsx_module_info* info)
  {
    info->identifier =
      "mesh;vertices;lightning_vertices";

    info->description =
      "Generates a line with random offset\n"
      "vertices from one mesh to another."
    ;

    info->in_param_spec =
      "rand_seed:float,"
      "lifetime:float,"
      "length:float,"
      "mesh_a:mesh,"
      "num_points:float,"
      "scaling:float3?nc=1"
    ;

    info->out_param_spec =
      "mesh:mesh";

    info->component_class =
      "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    rand_seed = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"rand_seed");
    length = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"length");
    length->set(1.0f);
    lifetime_mod = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"lifetime");
    lifetime_mod->set(1.0f);
    num_points = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"num_points");
    num_points->set(100);
    scaling = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"scaling");
    scaling->set(1,0);
    scaling->set(1,1);
    scaling->set(1,2);
    mesh_a = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_a");
    result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh");
    first_run = true;
    lifetime = 0;
  }

  bool init() {
    mesh = new vsx_mesh<>;
    return true;
  }

  void on_delete()
  {
    delete mesh;
  }

  void run()
  {
    vsx_mesh<>** p_a = mesh_a->get_addr();

    if (p_a)
    {
      if (lifetime <= 0)
      {

        id_a = rand.rand() % (*p_a)->data->vertices.size();
        lifetime = (int) ((float)(rand.rand() % 10) * lifetime_mod->get()) + 5;
        delta = (*p_a)->data->vertex_normals[id_a] * rand.frand() * length->get();
        start = (*p_a)->data->vertices[id_a];
        delta *= 1.0f / (float)((int)num_points->get());
      }

      int i;
      start = (*p_a)->data->vertices[id_a];
      float one_div_num_points = 1.0f / (float)((int)num_points->get());
      float sx = scaling->get(0) * length->get();
      float sy = scaling->get(1) * length->get();
      float sz = scaling->get(2) * length->get();
      for (i = 0; i < (int)num_points->get(); ++i)
      {
        mesh->data->vertices[i].x = start.x + (rand.frand()-0.5f)*sx * sin((float)i * one_div_num_points * PI);
        mesh->data->vertices[i].y = start.y + (rand.frand()-0.5f)*sy * sin((float)i * one_div_num_points * PI);
        mesh->data->vertices[i].z = start.z + (rand.frand()-0.5f)*sz * sin((float)i * one_div_num_points * PI);
        float c = 1.0f - (float)i * one_div_num_points;
        mesh->data->vertex_colors[i].r = c;
        mesh->data->vertex_colors[i].g = c;
        mesh->data->vertex_colors[i].b = c;
        mesh->data->vertex_colors[i].a = 1.0f;
        start = mesh->data->vertices[i];
        start += delta;
      }
      mesh->data->vertices.reset_used(i);
      lifetime--;
      result->set(mesh);
    }
  }

  ~module_mesh_lightning_vertices()
  {
  }
};
