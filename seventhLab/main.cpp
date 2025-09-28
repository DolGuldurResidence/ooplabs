#include "dependency_injection.h"
#include <iostream>
#include <memory>

int main() {
    std::cout << "=== Debug Configuration ===" << std::endl;
    Injector inj;
    config_debug(inj);
    
    // Test PerRequest
    auto i1a = inj.get_instance<Interface1>();
    auto i1b = inj.get_instance<Interface1>();
    std::cout << "PerRequest instances equal? " << (i1a.get() == i1b.get() ? "true" : "false") << std::endl; // false
    
    // Test Scoped
    inj.create_scope();
    auto s1 = inj.get_instance<Interface2>();
    auto s2 = inj.get_instance<Interface2>();
    std::cout << "Scoped instances same in scope? " << (s1.get() == s2.get() ? "true" : "false") << std::endl; // true
    inj.end_scope();
    
    // Test Singleton
    auto singleton1 = inj.get_instance<Interface3>();
    auto singleton2 = inj.get_instance<Interface3>();
    std::cout << "Singleton instances same? " << (singleton1.get() == singleton2.get() ? "true" : "false") << std::endl; // true
    
    std::cout << "\n=== Release Configuration ===" << std::endl;
    Injector inj2;
    config_release(inj2);
    
    inj2.create_scope();
    auto instance = inj2.get_instance<Interface1>();
    instance->do_work();
    
    auto calculator = inj2.get_instance<Interface2>();
    int result = calculator->calculate(5, 6);
    std::cout << "Calculation result: " << result << std::endl;
    inj2.end_scope();
    
    auto reporter = inj2.get_instance<Interface3>();
    reporter->report();
    
    std::cout << "\n=== Complex Dependency Demo ===" << std::endl;
    Injector inj3;
    config_complex(inj3);
    
    // Создание UserService с автоматическим разрешением зависимостей
    auto user_service = inj3.get_instance<UserService>();
    std::string user = user_service->get_user(42);
    std::cout << "Retrieved: " << user << std::endl;
    
    // Проверка разных экземпляров для PER_REQUEST
    auto logger1 = inj3.get_instance<Logger>();
    auto logger2 = inj3.get_instance<Logger>();
    std::cout << "Different logger instances? " << (logger1.get() != logger2.get() ? "true" : "false") << std::endl; // true
    
    std::cout << "\n=== Demonstration of all interfaces ===" << std::endl;
    
    // Демонстрация работы с debug конфигурацией
    std::cout << "\nDebug mode demonstration:" << std::endl;
    auto debug_worker = inj.get_instance<Interface1>();
    debug_worker->do_work();
    
    inj.create_scope();
    auto debug_calc = inj.get_instance<Interface2>();
    int debug_result = debug_calc->calculate(10, 20);
    std::cout << "Debug calculation result: " << debug_result << std::endl;
    inj.end_scope();
    
    auto debug_reporter = inj.get_instance<Interface3>();
    debug_reporter->report();
    
    // Демонстрация работы с release конфигурацией
    std::cout << "\nRelease mode demonstration:" << std::endl;
    auto release_worker = inj2.get_instance<Interface1>();
    release_worker->do_work();
    
    inj2.create_scope();
    auto release_calc = inj2.get_instance<Interface2>();
    int release_result = release_calc->calculate(10, 20);
    std::cout << "Release calculation result: " << release_result << std::endl;
    inj2.end_scope();
    
    auto release_reporter = inj2.get_instance<Interface3>();
    release_reporter->report();
    
    std::cout << "\n=== LifeStyle demonstration ===" << std::endl;
    
    // Демонстрация различных жизненных циклов
    std::cout << "\nPerRequest lifestyle (new instance each time):" << std::endl;
    auto per_req1 = inj.get_instance<Interface1>();
    auto per_req2 = inj.get_instance<Interface1>();
    std::cout << "Different instances: " << (per_req1.get() != per_req2.get() ? "true" : "false") << std::endl;
    
    std::cout << "\nScoped lifestyle (same instance within scope):" << std::endl;
    inj.create_scope();
    auto scoped1 = inj.get_instance<Interface2>();
    auto scoped2 = inj.get_instance<Interface2>();
    std::cout << "Same instance in scope: " << (scoped1.get() == scoped2.get() ? "true" : "false") << std::endl;
    inj.end_scope();
    
    std::cout << "\nSingleton lifestyle (always same instance):" << std::endl;
    auto singleton_a = inj.get_instance<Interface3>();
    auto singleton_b = inj.get_instance<Interface3>();
    std::cout << "Always same instance: " << (singleton_a.get() == singleton_b.get() ? "true" : "false") << std::endl;
    
    return 0;
}
