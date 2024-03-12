#pragma once

#include <gmock/gmock.h>

#include <functional>
#include <memory>

template <typename Product, typename... ArgTypes>
struct MockFactory;

/* Changes factory std::function into mock, reverts change upon destruction. */

template <typename Product, typename... ArgTypes>
struct MockFactory<std::function<std::unique_ptr<Product>(ArgTypes...)>>
    : public testing::MockFunction<std::unique_ptr<Product>(ArgTypes...)>
{
    MockFactory(std::function<std::unique_ptr<Product>(ArgTypes...)>& currentFactoryStdFunction)
        : originalFactoryRef(currentFactoryStdFunction)
    {
        // Store production factory and replace it with mock.
        originalFactoryValue = currentFactoryStdFunction;
        originalFactoryRef = this->AsStdFunction();
    }

    ~MockFactory()
    {
        // Restore production factory funciton.
        originalFactoryRef = originalFactoryValue;
    }

    std::function<std::unique_ptr<Product>(ArgTypes...)>& originalFactoryRef;
    std::function<std::unique_ptr<Product>(ArgTypes...)> originalFactoryValue;
};
