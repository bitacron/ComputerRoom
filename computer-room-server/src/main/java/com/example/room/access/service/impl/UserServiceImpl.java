package com.example.room.access.service.impl;

import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.baomidou.mybatisplus.extension.service.impl.ServiceImpl;
import com.example.room.access.entity.User;
import com.example.room.access.entity.UserRole;
import com.example.room.access.mapper.UserMapper;
import com.example.room.access.service.UserRoleService;
import com.example.room.access.service.UserService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.data.redis.core.RedisTemplate;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.io.Serializable;
import java.util.Collection;

@Service
public class UserServiceImpl extends ServiceImpl<UserMapper, User> implements UserService {

    @Autowired
    private UserRoleService userRoleService;

    @Autowired
    private RedisTemplate<String, Object> redisTemplate;

    @Override
    public User selectByUsername(String username) {
        return baseMapper.selectOne(new QueryWrapper<User>().eq("username", username));
    }

    @Override
    @Transactional(rollbackFor = Exception.class)
    public boolean removeById(Serializable id) {
        User user = getById(id);
        userRoleService.remove(new QueryWrapper<UserRole>().eq("user_id", id));
        boolean removed = super.removeById(id);
        if (removed && user != null) {
            redisTemplate.delete(user.getUsername());
        }
        return removed;
    }

    @Override
    @Transactional(rollbackFor = Exception.class)
    public boolean removeByIds(Collection<? extends Serializable> idList) {
        boolean removed = true;
        for (Serializable id : idList) {
            removed = removeById(id) && removed;
        }
        return removed;
    }
}
