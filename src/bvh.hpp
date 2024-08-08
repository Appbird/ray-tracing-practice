#ifndef BVH_H
#define BVH_H

#include "rtweekend.hpp"

#include "aabb.hpp"
#include "hittable.hpp"
#include "hittable_list.hpp"

#include <algorithm>

class bvh_node : public hittable {
    public:
    bvh_node(
        std::vector<shared_ptr<hittable>>& objects,
        size_t start,
        size_t end
    ) {
        bbox = aabb::empty;
        for (size_t object_index = start; object_index < end; object_index++) {
            bbox = aabb(bbox, objects[object_index]->bounding_box());
        }
        int32_t axis = bbox.longest_axis();

        auto comparator = box_x_compare;
        switch (axis) {
            case 0: comparator = box_x_compare; break;
            case 1: comparator = box_y_compare; break;
            case 2: comparator = box_z_compare; break;
            default: assert(false);
        }
        
        size_t object_span = end - start;

        if (object_span == 1) {
            left = right = objects[start];
        } else if (object_span == 2) {
            left = objects[start];
            right = objects[start+1];
        } else {
            std::sort(
                objects.begin() + start,
                objects.begin() + end,
                comparator
            );
            auto mid = start + object_span / 2;
            left = make_shared<bvh_node>(objects, start, mid);
            right = make_shared<bvh_node>(objects, mid, end);
        }

        bbox = aabb(left->bounding_box(), right->bounding_box());
    }
    
    bvh_node(hittable_list list):
        bvh_node(list.objects, 0, list.objects.size())
    {}

    bool hit(
        const ray& r,
        interval ray_t,
        hit_record& rec
    ) const override {
        if (not bbox.hit(r, ray_t)) { return false; }
        
        const bool hit_left = left->hit(r, ray_t, rec);
        auto right_interval = interval{ray_t.min, hit_left ? rec.t : ray_t.max};
        const bool hit_right = right->hit(r, right_interval, rec);
        return hit_left or hit_right;
    }

    aabb bounding_box() const override {
        return bbox;
    }

    private:
        shared_ptr<hittable> left;
        shared_ptr<hittable> right;
        aabb bbox;

        static bool box_compare(
            const shared_ptr<hittable> a,
            const shared_ptr<hittable> b,
            int axis_index
        ) {
            interval a_axis_interval = a->bounding_box().axis_interval(axis_index);
            interval b_axis_interval = b->bounding_box().axis_interval(axis_index);
            return a_axis_interval.min < b_axis_interval.min;
        }
        using SH = shared_ptr<hittable>;
        static bool box_x_compare (const SH a, const SH b){
            return box_compare(a, b, 0);
        };
        static bool box_y_compare (const SH a, const SH b){
            return box_compare(a, b, 1);
        };
        static bool box_z_compare (const SH a, const SH b){
            return box_compare(a, b, 2);
        };
};

#endif