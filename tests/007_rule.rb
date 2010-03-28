require 'test/unit'
require 'rbclips'

class Test_Rule < Test::Unit::TestCase

  def teardown
    Clips::Base.reset
  end

  def test_exists
    assert Clips.constants.member?(:Rule)
    assert Clips::Rule.constants.member?(:Creator)
  end

  def test_new_block
    assert_raise(Clips::UsageError)     { Clips::Rule::Creator.new }
    assert_raise(Clips::ArgumentError)  { Clips::Rule.new(3){}  }

    rule = Clips::Rule.new 'animal-mammal' do |r|
      r.pattern "(animal ?a)"
      r.pattern "(child-of ?a ?b)"
      r.rhs "(assert (animal ?b))"
    end  
    assert_equal rule.to_s, "(defrule animal-mammal (animal ?a) (child-of ?a ?b) => (assert (animal ?b)))"
  end

  def test_creator_patter
    rule = Clips::Rule.new 'animal-mammal' do |r|
      r.pattern 'animal', :a
      r.rhs '(assert (animal ?a))'
    end
    assert_equal rule.to_s, "(defrule animal-mammal ?rbclips-0 <- (animal ?a) => (assert (animal ?a)))"

    rule = Clips::Rule.new 'animal-mammal' do |r|
      r.pattern 'animal', :one
    end
    assert_equal rule.to_s, "(defrule animal-mammal ?rbclips-0 <- (animal ?) => )"

    rule = Clips::Rule.new 'animal-mammal' do |r|
      r.pattern 'animal', :all
    end
    assert_equal rule.to_s, "(defrule animal-mammal ?rbclips-0 <- (animal $?) => )"

    rule = Clips::Rule.new 'animal-mammal' do |r|
      r.pattern 'animal', 1, "ahoj", 2.3
    end
    assert_equal rule.to_s, '(defrule animal-mammal ?rbclips-0 <- (animal 1 "ahoj" 2.3) => )'

    assert_raise(Clips::UsageError)  do
      Clips::Rule.new 'x' do |r|
      end
    end

    t = Clips::Template.new :name => 'human', :slots => %w(age name)
    rule = Clips::Rule.new 'humanize' do |r|
      r.pattern t, :age => :a, :name => 'jarcec'
    end
    assert_equal rule.to_s, '(defrule humanize ?rbclips-0 <- (human (age ?a) (name "jarcec")) => )'

    assert_raise(Clips::ArgumentError)  do
      Clips::Rule.new 'human' do |r|
        r.pattern t, :nick => 'jarcec'
      end
    end
  end

  def test_creator_assert
    rule = Clips::Rule.new 'ahoj' do |r|
      r.pattern 'zavadec', 'windows'
      r.assert 'go-to', 'mars'
    end
    assert_equal rule.to_s, '(defrule ahoj ?rbclips-0 <- (zavadec "windows") => (assert (go-to "mars")))'

    rule = Clips::Rule.new 'ahoj2' do |r|
      r.pattern 'mammal', :mammal
      r.assert 'animal', :mammal
    end
    assert_equal rule.to_s, '(defrule ahoj2 ?rbclips-0 <- (mammal ?mammal) => (assert (animal ?mammal)))'

    tmpl = Clips::Template.new "tmpl", %w{a b c}

    rule = Clips::Rule.new 'ahoj3' do |r|
      r.pattern 'a', :b
      r.assert tmpl, :b => :b
    end
    assert_equal rule.to_s, '(defrule ahoj3 ?rbclips-0 <- (a ?b) => (assert (tmpl (b ?b))))'
  end

  def test_creator_retract
    rule = Clips::Rule.new 'animal-mammal' do |r|
      r.retract 'animal', :a
    end
    assert_equal rule.to_s, '(defrule animal-mammal ?rbclips-0 <- (animal ?a) => (retract ?rbclips-0))'

    rule = Clips::Rule.new 'animal-mammal' do |r|
      r.retract 'animal', :a
      r.retract 'mammal', :a
      r.retract 'child-of', :a
    end
    assert_equal rule.to_s, '(defrule animal-mammal ?rbclips-0 <- (animal ?a) ?rbclips-1 <- (mammal ?a) ?rbclips-2 <- (child-of ?a) => (retract ?rbclips-0) (retract ?rbclips-1) (retract ?rbclips-2))'

    t = Clips::Template.new :name => 'human', :slots => %w(age name)
    rule = Clips::Rule.new 'humanize' do |r|
      r.retract t, :age => :a, :name => 'jarcec'
    end
    assert_equal rule.to_s, '(defrule humanize ?rbclips-0 <- (human (age ?a) (name "jarcec")) => (retract ?rbclips-0))'
  end

  def test_creator_or_and_not
    rule = Clips::Rule.new 'x' do |r|
      r.retract 'a', :a
      r.or do |o|
        o.retract 'b', :a
        o.retract 'c', :a
      end
    end
    assert_equal rule.to_s, '(defrule x ?rbclips-0 <- (a ?a) (or ?rbclips-1 <- (b ?a) ?rbclips-2 <- (c ?a)) => (retract ?rbclips-0) (retract ?rbclips-1) (retract ?rbclips-2))'

    rule = Clips::Rule.new 'x' do |r|
      r.retract 'a', :a
      r.or do |o|
        o.retract 'b', :a
        o.and do |a|
          a.retract 'c', :a
          a.retract 'd', :a
        end
      end
      r.retract 'd', :a
      r.not {|n| n.pattern 'e', :a }
    end

    assert_raise(Clips::UsageError)  do
      Clips::Rule.new 'x' do |r|
        r.or {|o| }
      end
    end

    # Checking existance of methods
    rule = Clips::Rule.new 'x' do |r|
      assert_respond_to r, :pattern
      assert_respond_to r, :retract
      assert_respond_to r, :and
      assert_respond_to r, :or
      assert_respond_to r, :not
      assert_respond_to r, :rhs
      assert_respond_to r, :assert
      assert_respond_to r, :rcall

      r.or do |o|
        assert_respond_to o, :pattern
        assert_respond_to o, :retract
        assert_respond_to o, :and
        assert_respond_to o, :or
        assert_respond_to o, :not
        assert_raise(NoMethodError) { o.rhs "a" }
        assert_raise(NoMethodError) { o.assert "a" }
        assert_raise(NoMethodError) { o.rcall "a" }

        o.pattern :a, :b
      end

      r.not do |n|
        assert_respond_to n, :pattern
        assert_respond_to n, :and
        assert_respond_to n, :or
        assert_raise(NoMethodError) { n.rhs "a" }
        assert_raise(NoMethodError) { n.assert "a" }
        assert_raise(NoMethodError) { n.rcall "a" }
        assert_raise(NoMethodError) { n.retract "a" }
        assert_raise(NoMethodError) { n.not "a" }

        n.pattern :b, :c

        n.and do |a|
          assert_respond_to a, :pattern
          assert_respond_to a, :and
          assert_respond_to a, :or
          assert_raise(NoMethodError) { a.rhs "a" }
          assert_raise(NoMethodError) { a.assert "a" }
          assert_raise(NoMethodError) { a.rcall "a" }
          assert_raise(NoMethodError) { a.retract "a" }
          assert_raise(NoMethodError) { a.not "a" }
        
          a.pattern :x, :y
        end
      end
    end

  end

  def test_save_destroy!
    rule = Clips::Rule.new 'animal-mammal' do |r|
      r.pattern "(animals ?a)"
      r.pattern "(child-of ?a ?b)"
      r.rhs "(assert (animals ?b))"
    end  
    assert rule.save
    assert rule.destroy!
    assert !rule.destroy!
  end

  def test_load
    rule = Clips::Rule.new 'animal-mammal' do |r|
      r.pattern "(animals ?a)"
      r.pattern "(child-of ?a ?b)"
      r.rhs "(assert (animals ?b))"
    end  
    assert rule.save
    copy = Clips::Rule.load 'animal-mammal'
    assert copy.destroy!
    assert !rule.destroy!
  end

  def test_equal
    a = Clips::Rule.new 'animal-mammal-da' do |r|
      r.pattern "(animals ?a)"
      r.pattern "(child-of ?a ?b)"
      r.rhs "(assert (animals ?b))"
    end  
    assert_raise(Clips::UsageError) { assert_equal a, a }
    assert a.save

    b = Clips::Rule.load 'animal-mammal-da'
    assert_equal a, b

    assert_not_equal a, 3
    assert_not_equal b, "ahoj"

    assert a.destroy!
  end

  def test_all
    a = Clips::Rule.new 'animal-mammal' do |r|
      r.pattern "(animals ?a)"
      r.pattern "(child-of ?a ?b)"
      r.rhs "(assert (animals ?b))"
    end  
    b = Clips::Rule.new 'animal-mammal2' do |r|
      r.pattern "(animals ?a)"
      r.pattern "(child-of ?a ?b)"
      r.rhs "(assert (animals ?b))"
    end  
    assert a.save
    assert b.save

    assert_equal [a, b], Clips::Rule.all
  end

  def test_factaddress
    rule = Clips::Rule.new 'dump' do |r|
      a = r.pattern :a, :p1
      assert_equal a.to_s, "?rbclips-0"
      assert_instance_of Clips::FactAddress, a

      a = r.retract :b, :p1
      assert_equal a.to_s, "?rbclips-1"
      assert_instance_of Clips::FactAddress, a

      r.or do |o|
        a = o.pattern :c, :p1
        assert_equal a.to_s, "?rbclips-2"
      end

      a = r.retract :d, :p1
      assert_equal a.to_s, "?rbclips-3"

      r.and do |o|
        a = o.pattern :e, :p1
        assert_equal a.to_s, "?rbclips-4"
      end

      a = r.retract :f, :p1
      assert_equal a.to_s, "?rbclips-5"
    end
  end

  # Dump class for testing purposes
  class Dump 
    def initialize
      @num = 0
    end
    def add(num)
      @num += num
    end
  end

  def test_rcall_base
    dump = Dump.new

    rule = Clips::Rule.new "counter" do |r|
      r.pattern 'add', :number
      r.rcall dump, :add, :number
    end
    rule.save
    Clips::Fact.new("add", [3]).save
    Clips::Fact.new("add", [5]).save
    Clips::Base.run

    assert_equal dump.instance_eval{@num}, 8
  end

  def fact(fact)
    assert_instance_of Clips::Fact, fact
  end

  def test_rcall_fact
    rule = Clips::Rule.new "factCaller" do |r|
      addr = r.pattern 'f', :number
      r.rcall self, :fact, addr
    end

    assert rule.save
    Clips::Fact.new("f", [3]).save
    Clips::Fact.new("f", [5]).save
    Clips::Base.run
  end

  #
  # Creating fact instace methods 
  #
 def subtest_instance_methods_ordered(fact)
    assert_instance_of Clips::Fact, fact
    assert_respond_to fact, :name
    assert_respond_to fact, :slots
  end
  def test_instance_methods_ordered
    rule = Clips::Rule.new "factInstanceMethods" do |r|
      addr = r.pattern 'f', :number
      r.rcall self, :subtest_instance_methods_ordered, addr
    end
    assert rule.save

    Clips::Fact.new("f", [3]).save
    Clips::Base.run
  end

  def subtest_instance_methods_nonordered(fact)
    assert_instance_of Clips::Fact, fact
    assert_respond_to fact, :template
    assert_respond_to fact, :slot
    assert_respond_to fact, :name
    assert_respond_to fact, :age
    assert_respond_to fact, :race
  end
  def test_instance_methods_nonordered
    template = Clips::Template.new 'animal' do |t|
      t.slot :name
      t.slot :age
      t.slot :race
    end
    assert template.save

    rule = Clips::Rule.new "factInstanceMethods" do |r|
      addr = r.pattern template, {}
      r.rcall self, :subtest_instance_methods_nonordered, addr
    end
    assert rule.save

    Clips::Fact.new(template, :name => "Azor", :race => "dog", :age => 2).save
    Clips::Base.run
  end
 
end
